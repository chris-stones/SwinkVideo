

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct arguments
{
  int verbose;
  int quiet;
  int format;
  int lz4hc;
  char *output_file;
  char *log_file;
  char *resources;
  int debug;
};

typedef struct arguments arguments_t;

arguments_t read_args(int argc, char ** argv );

#ifdef __cplusplus
} // extern "C" {
#endif

