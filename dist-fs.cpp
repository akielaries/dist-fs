#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>

#include <cstdlib>
#include <stdio.h>  // I prefer printf

#include "dist-fs/file_format.hpp"

std::string hex_to_ascii(unsigned int hexValue) {
  std::ostringstream oss;
  for (int i = sizeof(hexValue) - 1; i >= 0; --i) {
    char ch = (hexValue >> (i * 8)) & 0xFF;
    if (ch) {
      oss << ch;
    }
  }
  return oss.str();
}

int main(int argc, char *argv[]) {
  printf("dist-fs app\n\n");

  if (argc < 2) {
    printf("Pass in a file to read...\n");
    return -1;
  }

  // read in the file (argv[1]) as a binary file
  std::ifstream audio_file(argv[1]);
  if (!audio_file) {
    printf("No such file %s\n", argv[1]);
  }

  std::vector<char> audio_file_data((std::istreambuf_iterator<char>(audio_file)),
                              std::istreambuf_iterator<char>());  
  if (audio_file_data.size() < DIST_FS_RIFF_SZ) {
    printf("File is too small to contain a valid RIFF header... perhaps this should be different\n");
  }

  std::cout << "Audio file data: " << std::endl;
  for (uint8_t i = 0; i < DIST_FS_RIFF_SZ ; i++) {
    std::cout << audio_file_data[i];
  }


  std::string riff = hex_to_ascii(DIST_FS_RIFF);

  printf("RIFF chunk desc: \n");
  printf("\tChunk ID: 0x%0X (%s) \n", DIST_FS_RIFF, riff.c_str() );

  
  audio_file.close();

  return 0;
}
