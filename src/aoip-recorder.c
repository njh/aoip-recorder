/*

  aoip-recorder.c

  Copyright (C) 2018  Nicholas Humfrey
  License: MIT

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "aoip-recorder.h"


const char *default_port = "5004";
const int default_sample_rate = 48000;
const int default_sample_size = 24;
const int default_channel_count = 2;
const int default_packet_buffer_size = 4;
const float default_file_duration = 10.0;


// Globals
ar_config_t config;
ar_socket_t sock;


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
    fprintf(stderr, "   -v             Verbose Logging\n");
    fprintf(stderr, "   -q             Quiet Logging\n");

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
        ar_error("Invalid sample format: %s", fmt);
        exit(-1);
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
    const char* progname = argv[0];
    int ch;

    // Parse the options/switches
    while ((ch = getopt(argc, argv, "a:p:i:r:f:c:b:d:vq?h")) != -1) {
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
        case 'v':
            verbose = TRUE;
            break;
        case 'q':
            quiet = TRUE;
            break;
        case '?':
        case 'h':
        default:
            usage(progname);
        }
    }

    // Check remaining arguments
    argc -= optind;
    argv += optind;
    if (argc > 1) {
        usage(progname);
    }
    
    // Validate parameters
    if (quiet && verbose) {
        ar_error("Can't be quiet and verbose at the same time.");
        usage(progname);
    }

    if (config->address == NULL || strlen(config->address) < 1) {
        ar_error("No address specified");
        usage(progname);
    }
}

int main(int argc, char *argv[])
{
    int result;

    set_config_defaults(&config);
    parse_opts(argc, argv, &config);
    setup_signal_hander();

    result = ar_socket_open(&sock, &config);
    if (result) {
        return EXIT_FAILURE;
    }

    while(running) {
      char buffer[2048];

      int len = ar_socket_recv(&sock, buffer, sizeof(buffer));
      ar_info("Got packet: %d", len);
      if (len < 1) break;
    }

    ar_socket_close(&sock);

    return exit_code;
}
