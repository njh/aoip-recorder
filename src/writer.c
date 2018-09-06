/*
  writer.c

  AoIP Recorder
  Copyright (C) 2018  Nicholas Humfrey
  License: MIT
*/

#include <stdlib.h>
#include <sndfile.h>

#include "aoip-recorder.h"



SNDFILE * ar_writer_open( ar_config_t *config, const char* path)
{
    SF_INFO sfinfo;

    sfinfo.format = SF_FORMAT_WAV | SF_ENDIAN_FILE;
    sfinfo.samplerate = config->sample_rate;
    sfinfo.channels = config->channel_count;
    
    switch (config->sample_size) {
        case 16:
            sfinfo.format |= SF_FORMAT_PCM_16;
        break;
        case 24:
            sfinfo.format |= SF_FORMAT_PCM_24;
        break;
        case 32:
            sfinfo.format |= SF_FORMAT_PCM_32;
        break;
        default:
            ar_error("Unsupported sample size: %d", config->sample_size);
            return NULL;
        break;
    }

    // Check that the format is valid
    if (!sf_format_check(&sfinfo)) {
        ar_error( "Output format is not valid." );
        return NULL;
    }
    
    return sf_open(path, SFM_WRITE, &sfinfo);
}

#define bytesToInt24(a) (((int)(a)[0] << 24) | \
                         ((int)(a)[1] << 16) | \
                         ((int)(a)[2] << 8))

void ar_writer_write(SNDFILE *file, uint8_t* payload, int payload_length)
{
    int i;
    
    if (payload_length % 6 != 0) {
        ar_warn("payload size is not a multiple of 6");
    }

    for(i=0; i< payload_length; i+=6) {
        sf_count_t written;
        int s32[2];

        s32[0] = bytesToInt24(&payload[i]);
        s32[1] = bytesToInt24(&payload[i+3]);
    
        written = sf_write_int(file, s32, 2);
        if (written != 2) {
            ar_error("Failed to write audio to disk: %s", sf_strerror(file));
        }
    }
}
