/*

  aoip-recorder.h

  Copyright (C) 2018  Nicholas Humfrey
  License: MIT

*/

#include "config.h"

#ifndef AOIP_RECORDER_H
#define AOIP_RECORDER_H


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



#endif
