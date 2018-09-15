/*
  writer.c

  AoIP Recorder
  Copyright (C) 2018  Nicholas Humfrey
  License: MIT
*/

#include <stdlib.h>
#include <sndfile.h>

#include "aoip-recorder.h"
#include "bytestoint.h"



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

void ar_writer_write(SNDFILE *file, uint8_t* payload, int payload_length)
{
    int s32[RTP_MAX_PAYLOAD / 3];
    sf_count_t written = 0;
    sf_count_t count = 0;
    int byte;

    if (payload_length > RTP_MAX_PAYLOAD) {
        ar_error("payload length is greater than maximum RTP payload size");
        return;
    }

    if (payload_length % 3 != 0) {
        ar_warn("payload length is not a multiple of 3");
    }

    // Convert payload to an array of 32-bit integers
    for(byte=0; byte < payload_length; byte += 3) {
        s32[count++] = bytesToInt24(&payload[byte]);
    }

    written = sf_write_int(file, s32, count);
    if (written != count) {
        ar_error("Failed to write audio to disk: %s", sf_strerror(file));
    }
}
