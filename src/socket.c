/*
  socket.c

  AoIP Recorder
  Copyright (C) 2018  Nicholas Humfrey
  License: MIT
*/

#include "config.h"
#include "aoip-recorder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#include <sys/time.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>



// Added to ensure compilation with KAME
#ifndef IPV6_ADD_MEMBERSHIP
#ifdef  IPV6_JOIN_GROUP
#define IPV6_ADD_MEMBERSHIP IPV6_JOIN_GROUP
#define IPV6_DROP_MEMBERSHIP IPV6_LEAVE_GROUP
#endif
#endif


static int _bind_socket(ar_socket_t *sock, const char* address, const char* port)
{
    struct addrinfo hints, *res, *cur;
    int error = -1;
    int retval = -1;

    // Setup hints for getaddrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    error = getaddrinfo(address, port, &hints, &res);
    if (error || res == NULL) {
        ar_warn("getaddrinfo failed: %s", gai_strerror(error));
        return error;
    }

    // Check each of the results
    cur = res;
    while (cur) {
        sock->fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);

        if (sock->fd >= 0) {
            int one = 1;
            // These socket options help re-binding to a socket
            // after a previous process was killed

#ifdef SO_REUSEADDR
            if (setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))) {
                perror("SO_REUSEADDR failed");
            }
#endif

#ifdef SO_REUSEPORT
            if (setsockopt(sock->fd, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one))) {
                perror("SO_REUSEPORT failed");
            }
#endif

            if (bind(sock->fd, cur->ai_addr, cur->ai_addrlen) == 0) {
                // Success!
                memcpy( &sock->saddr, cur->ai_addr, cur->ai_addrlen );
                retval = 0;
                break;
            }

            close(sock->fd);
        }
        cur=cur->ai_next;
    }

    freeaddrinfo( res );

    return retval;
}


static int _is_multicast(struct sockaddr_storage *addr)
{

    switch (addr->ss_family) {
    case AF_INET: {
        struct sockaddr_in *addr4=(struct sockaddr_in *)addr;
        return IN_MULTICAST(ntohl(addr4->sin_addr.s_addr));
    }
    break;

    case AF_INET6: {
        struct sockaddr_in6 *addr6=(struct sockaddr_in6 *)addr;
        return IN6_IS_ADDR_MULTICAST(&addr6->sin6_addr);
    }
    break;

    default: {
        return -1;
    }
    }
}

static int _join_group( ar_socket_t *sock )
{
    int retval = -1;

    switch (sock->saddr.ss_family) {
    case AF_INET:

        sock->imr.imr_multiaddr.s_addr=
            ((struct sockaddr_in*)&sock->saddr)->sin_addr.s_addr;
        sock->imr.imr_interface.s_addr= INADDR_ANY;

        retval= setsockopt(sock->fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                           &sock->imr, sizeof(sock->imr));
        if (retval<0)
            perror("_join_group failed on IP_ADD_MEMBERSHIP");
        break;

    case AF_INET6:

        memcpy(&sock->imr6.ipv6mr_multiaddr,
               &(((struct sockaddr_in6*)&sock->saddr)->sin6_addr),
               sizeof(struct in6_addr));

        // FIXME: should be a method for chosing interface to use
        sock->imr6.ipv6mr_interface=0;

        retval= setsockopt(sock->fd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP,
                           &sock->imr6, sizeof(sock->imr6));
        if (retval<0)
            perror("_join_group failed on IPV6_ADD_MEMBERSHIP");

        break;
    }

    return retval;
}


static int _leave_group( ar_socket_t* sock )
{ 
    int retval = -1;

    switch (sock->saddr.ss_family) {
    case AF_INET: {
        retval= setsockopt(sock->fd, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                           &(sock->imr), sizeof(sock->imr));
        if (retval<-1)
            perror("IP_DROP_MEMBERSHIP failed");
    }
    break;

    case AF_INET6: {
        retval= setsockopt(sock->fd, IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP,
                           &(sock->imr6), sizeof(sock->imr6));
        if (retval<-1)
            perror("IPV6_DROP_MEMBERSHIP failed");
    }
    break;

    }

    return retval;
}


int ar_socket_open(ar_socket_t* sock, ar_config_t *config)
{
    // Initialise
    memset(sock, 0, sizeof(ar_socket_t));
    sock->fd = 0;
    sock->is_multicast = 0;	// not joined yet

    ar_info("Opening socket: %s/%s", config->address, config->port);

    if (_bind_socket(sock, config->address, config->port)) {
        ar_error("Failed to open socket.");
        return -1;
    }

    // Join multicast group ?
    sock->is_multicast = _is_multicast( &sock->saddr );
    if (sock->is_multicast == 1) {

        ar_debug("Joining multicast group");
        if (_join_group(sock)) {
            ar_error("Failed to join multicast group.");
            sock->is_multicast = 0;
            ar_socket_close(sock);
            return -1;
        }

    } else if (sock->is_multicast != 0) {
        ar_error("Error checking if address is multicast");
    }

    return 0;
}


int ar_socket_recv( ar_socket_t* sock, void* data, unsigned  int len)
{
    fd_set readfds;
    struct timeval timeout;
    int packet_len, retval;

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    // Watch socket to see when it has input.
    FD_ZERO(&readfds);
    FD_SET(sock->fd, &readfds);
    retval = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);

    // Check return value
    if (retval == -1) {
        perror("select()");
        return -1;

    } else if (retval==0) {
        ar_warn("Timed out waiting for packet after %ld seconds", timeout.tv_sec);
        return 0;
    }

    // Packet is waiting - read it in
    packet_len = recv(sock->fd, data, len, 0);

    return packet_len;
}


void ar_socket_close(ar_socket_t* sock )
{
    // Drop Multicast membership
    if (sock->is_multicast)
    {
        _leave_group( sock );
    }

    // Close the sockets
    if (sock->fd >= 0) {
        close(sock->fd);
        sock->fd = -1;
    }
}
