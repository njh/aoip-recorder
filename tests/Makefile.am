AM_CFLAGS = $(CHECK_CFLAGS) -DFIXTURE_DIR=\"$(top_srcdir)/tests/\" -I$(top_srcdir)/src
AM_LDFLAGS = $(CHECK_LIBS)

check_PROGRAMS = check_config.t
TESTS = $(check_PROGRAMS)

.tc.c:
	checkmk $< > $@ || rm -f $@

check_config_t_SOURCES = \
  check_config.tc \
  $(top_srcdir)/src/ar_config.c \
  $(top_srcdir)/src/utils.c
