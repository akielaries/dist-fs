#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>

#include "bytecrush.h"


int compress_text_lz77(const char *file_path);
int compress_audio_flaclike(const char *file_path);


/***************************** pattern checkers *******************************/
static int pattern_check_repeats(const uint8_t *buffer,
                                 size_t size,
                                 size_t *pattern_length) {
  if (size < 2) {
    return 0;
  }

  size_t i = 1;
  while (i < size && buffer[i] == buffer[0]) {
    i++;
  }

  if (i > 1) {
    *pattern_length = i;
    // repeat pattern found...
    return 1;
  }
  return 0;
}

/******************************************************************************/
long get_file_size(const char *filename) {
  struct stat st;
  if (stat(filename, &st) == 0)
    return st.st_size;
  return -1;
}

size_t rle_compress(int16_t *data, size_t size, uint8_t *output) {
  size_t out_index = 0;
  size_t i         = 0;

  while (i < size) {
    int16_t value      = data[i];
    uint8_t run_length = 1;

    while (i + run_length < size && data[i + run_length] == value &&
           run_length < 255) {
      run_length++;
    }

    output[out_index++] = (uint8_t)run_length;
    memcpy(&output[out_index], &value, sizeof(int16_t));
    out_index += sizeof(int16_t);

    i += run_length;
  }
  return out_index;
}

/******************************************************************************/
int bytecrush_compress(const char *file_path) {
  printf("bytecrushing file %s\n", file_path);
  char output_path[256];
  snprintf(output_path, sizeof(output_path), "%s.bcrush", file_path);
}

int main(int argc, char *argv[]) {

  bytecrush_compress(argv[1]);

  return 0;
}
