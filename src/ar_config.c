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
        ar_error("Invalid sample format: %s", fmt);
        exit(-1);
    }
}
