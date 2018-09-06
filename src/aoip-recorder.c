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


// Globals
ar_config_t config;
ar_socket_t sock;


static void usage(const char * progname)
{
    fprintf(stderr, "AoIP Recorder version %s\n\n", PACKAGE_VERSION);
    fprintf(stderr, "%s [options] [<file.sdp>]\n", progname);
    fprintf(stderr, "   -a <address>   IP Address\n");
    fprintf(stderr, "   -i <iface>     Interface Name to listen on\n");
    fprintf(stderr, "   -p <port>      Port Number (default %s)\n", DEFAULT_PORT);
    fprintf(stderr, "   -r <rate>      Sample Rate (default %d)\n", DEFAULT_SAMPLE_RATE);
    fprintf(stderr, "   -f <format>    Audio Format (default L%d)\n", DEFAULT_SAMPLE_SIZE);
    fprintf(stderr, "   -c <channels>  Channel Count (default %d)\n", DEFAULT_CHANNEL_COUNT);
    fprintf(stderr, "   -b <size>      Packet Buffer Size (default %d)\n", DEFAULT_PACKET_BUFFER_SIZE);
    fprintf(stderr, "   -d <duration>  File Duration (default %2.2f)\n", DEFAULT_FILE_DURATION);
    fprintf(stderr, "   -v             Verbose Logging\n");
    fprintf(stderr, "   -q             Quiet Logging\n");

    exit(EXIT_FAILURE);
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
            ar_config_set_sample_format(config, optarg);
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
    if (argc == 1) {
        ar_config_parse_sdp(config, argv[0]);
    } else if (argc > 1) {
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
    SNDFILE * file;
    int result;

    ar_config_set_defaults(&config);
    parse_opts(argc, argv, &config);
    setup_signal_hander();

    result = ar_socket_open(&sock, &config);
    if (result) {
        return EXIT_FAILURE;
    }

    file = ar_writer_open(&config, "recording.wav");
    if (file == NULL) {
        ar_error("Failed to open output file");
    }

    while(running) {
        ar_rtp_packet_t packet;

        int result = ar_rtp_recv(&sock, &packet);
        if (result < 0) break;

        // Is the Payload Type what we were expecting?
        if (config.payload_type == -1) {
            ar_config_set_payload_type(&config, packet.payload_type);
        } else if (config.payload_type != packet.payload_type) {
            ar_warn("Received unexpected Payload Type: %d", packet.payload_type);
        }

        ar_debug("RTP packet ts=%lu seq=%u", packet.timestamp, packet.sequence);

        ar_writer_write(file, packet.payload, packet.payload_length);
    }

    if (file) {
        sf_close(file);
    }

    ar_socket_close(&sock);

    return exit_code;
}
