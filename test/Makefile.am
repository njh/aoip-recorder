AM_CFLAGS = $(CHECK_CFLAGS) \
            -DFIXTURE_DIR=\"$(srcdir)/fixtures/\" \
            -I$(top_srcdir)/src
AM_LDFLAGS = $(CHECK_LIBS)

check_PROGRAMS = check_config.t check_rtp.t
TESTS = $(check_PROGRAMS)

.tc.c:
	checkmk $< > $@ || rm -f $@

check_config_t_SOURCES = \
  check_config.tc \
  $(top_srcdir)/src/ar_config.c \
  $(top_srcdir)/src/utils.c

check_rtp_t_SOURCES = \
  check_rtp.tc \
  $(top_srcdir)/src/rtp.c \
  $(top_srcdir)/src/socket.c \
  $(top_srcdir)/src/utils.c

EXTRA_DIST = \
  fixtures/aoip44.sdp \
  fixtures/mono.sdp \
  fixtures/xnode.sdp

CLEANFILES = *.c
