[![Build](https://github.com/njh/aoip-recorder/actions/workflows/build.yml/badge.svg)](https://github.com/njh/aoip-recorder/actions/workflows/build.yml)

AoIP Recorder
=============

A command-line tool for recording [AES67] audio streams to WAV files.

AES67 is the Audio Engineering Society's standard for interoperable high-performance
audio over IP networks. It sets out how existing standards are used together: linear 
PCM audio (L16 or L24) is carried in [RTP] over UDP, unicast or multicast, and each 
stream is described by an [SDP] session description.

You can give the stream details as command-line options, or load them from the
stream's SDP file. Because AES67 builds on plain RTP, `aoip-recorder` can also record
other RTP audio streams that use the same formats.


Features
--------

* Receives RTP audio over UDP, unicast or multicast
* Joins IPv4 and IPv6 multicast groups, optionally on a named network interface
* Reads the address, port, payload type and audio format from an SDP file (or from stdin)
* Picks up the payload type from the first packet if none was given
* Writes 16, 24 or 32-bit PCM WAV files using [libsndfile]


Status
------

This is an early work in progress (version 0.0.1). Current limitations:

* The output is always written to `recording.wav` in the current directory. An existing file with that name is overwritten.
* Packets are written in the order they arrive. Lost or out-of-order packets are not detected or corrected.
* The `-b` (packet buffer size) and `-d` (file duration) options are accepted but have no effect yet.
* For IPv4 multicast, `-i` joins the group using the first IPv4 address on that interface.


Building
--------

You need a C compiler, autoconf, automake, pkg-config and libsndfile (1.0.18 or newer).
The unit tests also need the [check] library, which provides `checkmk`. Without it,
the tests are skipped.

On Debian or Ubuntu:

    sudo apt install build-essential pkg-config automake autoconf libtool libsndfile1-dev check

On macOS with Homebrew:

    brew install autoconf automake pkg-config libsndfile check

Then build:

    ./autogen.sh
    make

`autogen.sh` generates the build system and runs `./configure`. The finished binary is `src/aoip-recorder`.

To run the unit tests:

    make check


Usage
-----

    aoip-recorder [options] [<file.sdp>]

| Option          | Description                                      | Default |
|-----------------|--------------------------------------------------|---------|
| `-a <address>`  | IP address to listen on (unicast or multicast)   | none    |
| `-i <iface>`    | Network interface to join the multicast group on | system default |
| `-p <port>`     | UDP port number                                  | `5004`  |
| `-r <rate>`     | Sample rate in Hz                                | `48000` |
| `-f <format>`   | Sample format: `L16`, `L24` or `L32`             | `L24`   |
| `-c <channels>` | Number of channels                               | `2`     |
| `-b <size>`     | Packet buffer size (not used yet)                | `4`     |
| `-d <duration>` | File duration (not used yet)                     | `10`    |
| `-v`            | Verbose logging                                  |         |
| `-q`            | Quiet logging                                    |         |

You must give an address, either with `-a` or with a `c=` line in an SDP file. Values
from the SDP file always take priority over command-line options. Use `-` as
the file name to read the SDP from stdin.

Recording from a multicast stream on a specific interface:

    aoip-recorder -a 239.65.125.63 -p 5004 -i eth0

Recording from a stream described by an SDP file:

    aoip-recorder -v stream.sdp

Here is an example SDP file, as advertised by a Dante device:

    v=0
    o=- 19868418 19868458 IN IP4 169.254.98.63
    s=AOIP44-serial-9999 : 2
    c=IN IP4 239.65.125.63/32
    t=0 0
    m=audio 5004 RTP/AVP 97
    a=rtpmap:97 L24/48000/2

Press `Ctrl-C` to stop recording.


License
-------

`aoip-recorder` is licensed under the terms of the MIT license.
See the [LICENSE](LICENSE) file for details.


[AES67]:      https://en.wikipedia.org/wiki/AES67
[RTP]:        https://datatracker.ietf.org/doc/html/rfc3550
[SDP]:        https://datatracker.ietf.org/doc/html/rfc4566
[libsndfile]: https://libsndfile.github.io/libsndfile/
[check]:      https://libcheck.github.io/check/
