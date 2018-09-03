/*

  aoip-recorder.c

  Copyright (C) 2018  Nicholas Humfrey
  License: MIT

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "aoip-recorder.h"


const char *default_port = "5004";
const int default_sample_rate = 48000;
const int default_sample_size = 24;
const int default_channel_count = 2;
const int default_packet_buffer_size = 4;
const float default_file_duration = 10.0;

ar_config_t config;

static void usage(const char * progname)
{
    fprintf(stderr, "AoIP Recorder version %s\n\n", PACKAGE_VERSION);
    fprintf(stderr, "%s [options] [<file.sdp>]\n", progname);
    fprintf(stderr, "   -a <address>   IP Address\n");
    fprintf(stderr, "   -i <iface>     Interface Name to listen on\n");
    fprintf(stderr, "   -p <port>      Port Number (default %s)\n", default_port);
    fprintf(stderr, "   -r <rate>      Sample Rate (default %d)\n", default_sample_rate);
    fprintf(stderr, "   -f <format>    Audio Format (default L%d)\n", default_sample_size);
    fprintf(stderr, "   -c <channels>  Channel Count (default %d)\n", default_channel_count);
    fprintf(stderr, "   -b <size>      Packet Buffer Size (default %d)\n", default_packet_buffer_size);
    fprintf(stderr, "   -d <duration>  File Duration (default %2.2f)\n", default_file_duration);

    exit(EXIT_FAILURE);
}

static int parse_sample_format(const char *fmt)
{
    if (strcmp(fmt, "L16") == 0) {
        return 16;
    } else if (strcmp(fmt, "L24") == 0) {
        return 24;
    } else if (strcmp(fmt, "L32") == 0) {
        return 32;
    } else {
        fprintf(stderr, "Invalid sample format: %s\n", fmt);
        exit(EXIT_FAILURE);
    }
}

static void set_config_defaults(ar_config_t *config)
{
    memset(config, 0, sizeof(*config));

    config->address = NULL;
    config->port = default_port;
    config->ifname = NULL;

    config->sample_rate = default_sample_rate;
    config->sample_size = default_sample_size;
    config->channel_count = default_channel_count;
    config->packet_buffer_size = default_packet_buffer_size;
    config->file_duration = default_file_duration;
}



static void parse_opts(int argc, char **argv, ar_config_t *config)
{
    int ch;


    // Parse the options/switches
    while ((ch = getopt(argc, argv, "a:p:i:r:f:c:b:d:?h")) != -1) {
        switch (ch) {
        case 'a':
            config->address = optarg;
            break;
        case 'p':
            config->port = optarg;
            break;
        case 'i':
            config->ifname = optarg;
            break;
        case 'r':
            config->sample_rate = atoi(optarg);
            break;
        case 'f':
            config->sample_size = parse_sample_format(optarg);
            break;
        case 'c':
            config->channel_count = atoi(optarg);
            break;
        case 'b':
            config->packet_buffer_size = atoi(optarg);
            break;
        case 'd':
            config->file_duration = atoi(optarg);
            break;
        case '?':
        case 'h':
        default:
            usage(argv[0]);
        }
    }

}

int main(int argc, char *argv[])
{
    set_config_defaults(&config);
    parse_opts(argc, argv, &config);

    return EXIT_SUCCESS;
}
