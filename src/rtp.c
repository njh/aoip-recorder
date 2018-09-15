/*
  rtp.c

  AoIP Recorder
  Copyright (C) 2018  Nicholas Humfrey
  License: MIT
*/

#include <stdlib.h>

#include "config.h"
#include "bytestoint.h"
#include "aoip-recorder.h"


#define bitMask(byte, mask, shift) ((byte & (mask << shift)) >> shift)


#define RTP_HEADER_LENGTH   (12)

int ar_rtp_parse( ar_rtp_packet_t* packet )
{
    int header_len = RTP_HEADER_LENGTH;

    // Byte 1
    packet->version = bitMask(packet->buffer[0], 0x02, 6);
    packet->padding = bitMask(packet->buffer[0], 0x01, 5);
    packet->extension = bitMask(packet->buffer[0], 0x01, 4);
    packet->csrc_count = bitMask(packet->buffer[0], 0x0F, 0);

    // Byte 2
    packet->marker = bitMask(packet->buffer[1], 0x01, 7);
    packet->payload_type = bitMask(packet->buffer[1], 0x7F, 0);

    // Bytes 3 and 4
    packet->sequence = bytesToInt16(&packet->buffer[2]);

    // Bytes 5-8
    packet->timestamp = bytesToInt32(&packet->buffer[4]);

    // Bytes 9-12
    packet->ssrc = bytesToInt32(&packet->buffer[8]);

    // Calculate the size of the payload
    // FIXME: skip over header extension
    header_len += (packet->csrc_count * 4);
    packet->payload_length = packet->length - header_len;
    packet->payload = packet->buffer + header_len;

    // FIXME: Remove padding from payload_length

    // Success
    return 0;
}


int ar_rtp_recv( ar_socket_t* socket, ar_rtp_packet_t* packet )
{
    int len = ar_socket_recv(socket, &packet->buffer, sizeof(packet->buffer));

    // Failure or too short to be an RTP packet?
    if (len <= RTP_HEADER_LENGTH) return -1;

    packet->length = len;

    return ar_rtp_parse(packet);
}
