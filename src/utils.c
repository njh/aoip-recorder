/*
  utils.c

  AoIP Recorder
  Copyright (C) 2018  Nicholas Humfrey
  License: MIT
*/

#include "config.h"
#include "aoip-recorder.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

int running = TRUE;
int exit_code = 0;
int quiet = 0;
int verbose = 0;

static void termination_handler(int signum)
{
    running = FALSE;
    switch(signum) {
        case SIGTERM: ar_info("Got termination signal"); break;
        case SIGINT:  ar_info("Got interupt signal"); break;
    }
    signal(signum, termination_handler);
}


void setup_signal_hander()
{
    signal(SIGTERM, termination_handler);
    signal(SIGINT, termination_handler);
    signal(SIGHUP, termination_handler);
}

void ar_log(ar_log_level level, const char *fmt, ...)
{
  time_t t = time(NULL);
  char *time_str;
  va_list args;

  // Display the message level
  switch(level) {
    case AR_LOG_DEBUG:
      if (!verbose)
        return;
      fprintf(stderr, "[DEBUG]   ");
    break;
    case AR_LOG_INFO:
      if (quiet)
        return;
      fprintf(stderr, "[INFO]    ");
    break;
    case AR_LOG_WARN:
      fprintf(stderr, "[WARNING] ");
    break;
    case AR_LOG_ERROR:
      fprintf(stderr, "[ERROR]   ");
    break;
    default:
      fprintf(stderr, "[UNKNOWN] ");
    break;
  }

  // Display timestamp
  time_str = ctime(&t);
  time_str[strlen(time_str) - 1] = 0; // remove \n
  fprintf(stderr, "%s  ", time_str);

  // Display the error message
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);

  // If an erron then stop
  if (level == AR_LOG_ERROR) {
    // Exit with a non-zero exit code if there was a fatal error
    exit_code++;
    if (running) {
      // Quit gracefully
      running = 0;
    } else {
      fprintf(stderr, "Fatal error while quiting; exiting immediately.\n");
      exit(-1);
    }
  }
}
