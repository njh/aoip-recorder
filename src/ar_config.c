/*
  ar_config.c

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
#include <errno.h>
#include <ctype.h>

void ar_config_set_defaults(ar_config_t *config)
{
    memset(config, 0, sizeof(*config));

    config->address = NULL;
    config->port = DEFAULT_PORT;
    config->ifname = NULL;

    config->payload_type = -1;
    config->sample_rate = DEFAULT_SAMPLE_RATE;
    config->sample_size = DEFAULT_SAMPLE_SIZE;
    config->channel_count = DEFAULT_CHANNEL_COUNT;
    config->packet_buffer_size = DEFAULT_PACKET_BUFFER_SIZE;
    config->file_duration = DEFAULT_FILE_DURATION;
}

void ar_config_set_sample_format(ar_config_t *config, const char *fmt)
{
    ar_debug("Setting sample format to: %s", fmt);
    if (strcmp(fmt, "L16") == 0) {
        config->sample_size = 16;
    } else if (strcmp(fmt, "L24") == 0) {
        config->sample_size = 24;
    } else if (strcmp(fmt, "L32") == 0) {
        config->sample_size = 32;
    } else {
        ar_error("Unsupported audio sample format: %s", fmt);
        exit(-1);
    }
}


static void sdp_connection_parse(ar_config_t *config, char* line)
{
    char *nettype = strsep(&line, " ");
    char *addrtype = strsep(&line, " ");
    char *addr = strsep(&line, " /");

    if (addr == NULL || strcmp(nettype, "IN") != 0) {
        ar_error("SDP net type is not 'IN': %s", nettype);
    }

    if (addr == NULL || (strcmp(addrtype, "IP4") != 0 && strcmp(addrtype, "IP6") != 0)) {
        ar_warn("SDP Address type is not IP4/IP6: %s", nettype);
    }

    if (addr == NULL || strlen(addr) > 7) {
        config->address = strdup(addr);
    } else {
        ar_error("Invalid connection address: %s", addr);
    }
}

static void sdp_media_parse(ar_config_t *config, char* line)
{
    char *media = strsep(&line, " ");
    char *port = strsep(&line, " ");
    char *proto = strsep(&line, " ");
    char *fmt = strsep(&line, " ");

    if (media == NULL || strcmp(media, "audio") != 0) {
        ar_error("SDP media type is not audio: %s", media);
    }

    if (port == NULL || strlen(port) > 2) {
        config->port = strdup(port);
    } else {
        ar_error("Invalid connection port: %s", port);
    }

    if (proto == NULL || strcmp(proto, "RTP/AVP") != 0) {
        ar_error("SDP transport protocol is not RTP/AVP: %s", proto);
    }

    if (fmt == NULL || strlen(fmt) > 2) {
        ar_error("SDP media format is not valid: %s", fmt);
    } else {
        config->payload_type = atoi(fmt);
        ar_debug("  SDP Payload Type: %d", config->payload_type);
    }
}

static void sdp_attribute_parse(ar_config_t *config, char* line)
{
    char *attr = strsep(&line, ":");

    if (strcmp(attr, "rtpmap") == 0) {
        char *pt = strsep(&line, " ");
        
        if (pt != NULL && atoi(pt) == config->payload_type) {
          char *format = strsep(&line, "/");
          char *sample_rate = strsep(&line, "/");
          char *channel_count = strsep(&line, "/");
          ar_config_set_sample_format(config, format);
          config->sample_rate = atoi(sample_rate);
          config->channel_count = atoi(channel_count);
        }
    }
}

void ar_config_parse_sdp(ar_config_t *config, const char* filename)
{
    FILE* file = NULL;

    if (strcmp(filename, "-") == 0) {
        file = stdin;
    } else {
        file = fopen(filename, "rb");
    }

    if (!file) {
        ar_error("Failed to open file: %s", strerror(errno));
        return;
    }

    while(!feof(file)) {
        char line[1024];
        char *result;
        int i;

        result = fgets(line, sizeof(line), file);
        if (result == NULL)
            break;

        // Remove whitespace from the end of the line
        for(i=strlen(line); i > 0; i--) {
            if (isspace(line[i]) || line[i] == '\0') {
                line[i] = '\0';
            } else {
                break;
            }
        }

        if (strlen(line) < 3) {
            ar_warn("Invalid line in SDP file: line is too short");
            continue;
        }

        if (line[1] != '=') {
            ar_warn("Invalid line in SDP file: second character of line isn't =");
            continue;
        }

        switch (line[0]) {
            case 'v':
                if (strcmp(&line[2], "0") != 0) {
                    ar_warn("SDP version number is not 0: %s", &line[2]);
                }
                break;

            case 'c':
                sdp_connection_parse(config, &line[2]);
                break;

            case 'm':
                sdp_media_parse(config, &line[2]);
                break;

            case 'a':
                sdp_attribute_parse(config, &line[2]);
                break;
        }
    }

    fclose(file);
}
