/*

  aoip-recorder.h

  Copyright (C) 2018  Nicholas Humfrey
  License: MIT

*/

#include "config.h"

#ifndef AOIP_RECORDER_H
#define AOIP_RECORDER_H


const char *default_port = "5004";
const int default_sample_rate = 48000;
const int default_sample_size = 24;
const int default_channel_count = 2;
const int default_packet_buffer_size = 4;
const float default_file_duration = 10.0;

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



#endif
