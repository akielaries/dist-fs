#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>

#include <cstdint>
#include <cstdlib>
#include <stdio.h>  // I prefer printf

#include "dist-fs/audio_files.hpp"
#include "dist-fs/utils.hpp"


int main(int argc, char *argv[]) {
  printf("dist-fs app\n\n");

  if (argc < 2) {
    printf("Pass in a file to read...\n");
    return -1;
  }

  dist_fs_file_types_e file_type = get_file_type(argv[1]);

  printf("Found file type: %d\n", file_type);


  return 0;
}
