/*

  aoip-recorder.h

  Copyright (C) 2018  Nicholas Humfrey
  License: MIT

*/

#include "config.h"

#ifndef AOIP_RECORDER_H
#define AOIP_RECORDER_H

#include <sys/types.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#ifndef TRUE
#define TRUE  (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif



typedef struct
{
    const char* address;
    const char* port;
    const char* ifname;

    int packet_type;
    int sample_rate;
    int sample_size;
    int channel_count;

    int packet_buffer_size;
    float file_duration;

} ar_config_t;


typedef struct
{
    int fd;
    int is_multicast;

    struct addrinfo ainfo;
    struct sockaddr_storage saddr;
    struct ipv6_mreq imr6;
    struct ip_mreq imr;

} ar_socket_t;


int ar_socket_open(ar_socket_t* sock, ar_config_t *config);
int ar_socket_recv(ar_socket_t* sock, void* data, unsigned  int len);
void ar_socket_close(ar_socket_t* sock);

#endif
