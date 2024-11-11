#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>

#include <cstdint>
#include <cstdlib>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "dist-fs/audio_files.hpp"
#include "dist-fs/storage.hpp"
#include "dist-fs/utils.hpp"

void print_usage(const char *program_name) {
  printf("Usage: %s [OPTIONS]\n", program_name);
  printf("Options:\n");
  printf("  -u, --upload <file>      Upload the specified file to the SSD\n");
  printf(
    "  -d, --download <file>    Download the specified file from the SSD\n");
  printf("  -D, --delete <file>      Delete the specified file from the SSD\n");
  printf("  -l, --list               List all files on the SSD\n");
  printf("  -S, --ssd_echo <pattern> Perform an echo test on the SSD with a "
         "specified hex pattern (up to 16 bytes)\n");
  printf("\nExamples:\n");
  printf("  %s --upload example.wav\n", program_name);
  printf("  %s --ssd_echo ABABABAB\n", program_name);
  printf("\nNote: <file> must be specified for upload, download, and delete "
         "operations.\n");
}

int hex_string_to_bytes(const char *hex_string,
                        uint8_t *buffer,
                        uint32_t buffer_size) {
  size_t hex_len = strlen(hex_string);
  if (hex_len % 2 != 0 || hex_len / 2 > buffer_size)
    return -1;

  for (size_t i = 0; i < hex_len; i += 2) {
    unsigned int byte;
    if (sscanf(hex_string + i, "%2x", &byte) != 1)
      return -1;
    buffer[i / 2] = (uint8_t)byte;
  }
  return (uint32_t)hex_len / 2;
}

int main(int argc, char *argv[]) {
  printf("dist-fs app\n\n");

  if (argc < 2) {
    LOG(ERR, "Expecting at least one argument.");
    print_usage(argv[0]);
    return -1;
  }

  int option;
  uint8_t ssd_pattern[DIST_FS_SSD_PATTERN_SZ] = {0};

  while ((option = getopt(argc, argv, "u:d:D:lS:")) != -1) {
    switch (option) {
      case 'u': // --upload
        if (optarg == NULL) {
          LOG(ERR, "Option -u requires a file argument.");
          print_usage(argv[0]);
          return -1;
        }
        upload_file(optarg);
        break;
      case 'd': // --download
        if (optarg == NULL) {
          LOG(ERR, "Option -d requires a file argument.");
          print_usage(argv[0]);
          return -1;
        }
        download_file(optarg);
        break;
      case 'D': // --delete
        if (optarg == NULL) {
          LOG(ERR, "Option -D requires a file argument.");
          print_usage(argv[0]);
          return -1;
        }
        delete_file(optarg);
        break;
      case 'l': // --list
        list_files();
        break;
      case 'S': // --ssd_echo
        if (optarg == NULL) {
          LOG(ERR, "Option -S requires a hex pattern argument.");
          print_usage(argv[0]);
          return -1;
        }
        if (hex_string_to_bytes(optarg, ssd_pattern, DIST_FS_SSD_PATTERN_SZ) ==
            -1) {
          LOG(ERR, "Invalid hex pattern for --ssd_echo: %s", optarg);
          return -1;
        }
        ssd_echo(ssd_pattern);
        break;
      default:
        LOG(ERR, "Unknown option: -%c", option);
        print_usage(argv[0]);
        return -1;
    }
  }

  if (optind < argc) {
    LOG(ERR, "Unexpected additional argument: %s", argv[optind]);
    print_usage(argv[0]);
    return -1;
  }

  return 0;
}
