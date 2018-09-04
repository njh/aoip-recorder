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


// ------- Logging ---------

void setup_signal_hander();
extern int running;
extern int exit_code;
extern int verbose;
extern int quiet;

typedef enum {
  AR_LOG_DEBUG,
  AR_LOG_INFO,
  AR_LOG_WARN,
  AR_LOG_ERROR
} ar_log_level;


void ar_log(ar_log_level level, const char *fmt, ...);

// Only display debug if verbose
#define ar_debug( ... ) \
		ar_log(AR_LOG_DEBUG, __VA_ARGS__ )

// Don't show info when quiet
#define ar_info( ... ) \
		ar_log(AR_LOG_INFO, __VA_ARGS__ )

#define ar_warn( ... ) \
		ar_log(AR_LOG_WARN, __VA_ARGS__ )

// All errors are fatal
#define ar_error( ... ) \
		ar_log(AR_LOG_ERROR, __VA_ARGS__ )


#endif
