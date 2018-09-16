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

#include <sndfile.h>


#define DEFAULT_PORT  "5004"
#define DEFAULT_SAMPLE_RATE  48000
#define DEFAULT_SAMPLE_SIZE  24
#define DEFAULT_CHANNEL_COUNT  2
#define DEFAULT_PACKET_BUFFER_SIZE  4
#define DEFAULT_FILE_DURATION  10.0

#define RTP_MAX_PAYLOAD     (1440)
#define RTP_HEADER_LENGTH   (12)


#ifndef TRUE
#define TRUE  (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif



typedef struct
{
    char* address;
    char* port;
    char* ifname;

    int payload_type;
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

    struct sockaddr_storage saddr;

    union {
      struct ipv6_mreq imr6;
      struct ip_mreq imr;
    };

} ar_socket_t;

typedef struct
{
    uint8_t version;
    uint8_t padding;
    uint8_t extension;
    uint8_t csrc_count;
    uint8_t marker;
    uint8_t payload_type;

    uint16_t sequence;
    uint32_t timestamp;
    uint32_t ssrc;

    uint16_t payload_length;
    uint8_t *payload;

    uint16_t length;
    uint8_t buffer[1500];

} ar_rtp_packet_t;


int ar_socket_open(ar_socket_t* sock, ar_config_t *config);
int ar_socket_recv(ar_socket_t* sock, void* data, unsigned  int len);
void ar_socket_close(ar_socket_t* sock);

void ar_config_set_defaults(ar_config_t *config);
void ar_config_set_address(ar_config_t *config, const char *address);
void ar_config_set_port(ar_config_t *config, const char *port);
void ar_config_set_ifname(ar_config_t *config, const char *ifname);
void ar_config_set_sample_format(ar_config_t *config, const char *fmt);
void ar_config_set_payload_type(ar_config_t *config, int payload_type);
void ar_config_parse_sdp(ar_config_t *config, const char* filename);
void ar_config_free(ar_config_t *config);

int ar_rtp_parse( ar_rtp_packet_t* packet );
int ar_rtp_recv( ar_socket_t* socket, ar_rtp_packet_t* packet );

SNDFILE *ar_writer_open( ar_config_t *config, const char* path);
void ar_writer_write(SNDFILE *file, uint8_t* payload, int payload_length);


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
