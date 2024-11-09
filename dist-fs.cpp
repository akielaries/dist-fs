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

dist_fs_file_types_e get_file_type(const char *filename) {
  // read in the audio file
  std::ifstream audio_file(filename, std::ios::binary);
  if (!audio_file) {
    printf("No such file %s\n", filename);
    return DIST_FS_TYPE_UNKNOWN;
  }

  std::vector<char> audio_file_data((std::istreambuf_iterator<char>(audio_file)),
                                    std::istreambuf_iterator<char>());

  if (audio_file_data.size() < DIST_FS_TYPE_SZ) {
    printf("File is too small to contain a valid RIFF header.\n");
    return DIST_FS_TYPE_UNKNOWN;
  }

  // save the file's first 4 bytes, this will determine what kind of file we want
  // to check for
  uint32_t file_chunk_id = (static_cast<unsigned char>(audio_file_data[0]) << 24) |
                           (static_cast<unsigned char>(audio_file_data[1]) << 16) |
                           (static_cast<unsigned char>(audio_file_data[2]) << 8) |
                           static_cast<unsigned char>(audio_file_data[3]);

  std::string ascii_chunk_id = hex_to_ascii(file_chunk_id);
  printf("File Chunk ID: 0x%08X (%s)\n", file_chunk_id, ascii_chunk_id.c_str());

  // check for RIFF identifier this is most likely to indicate a WAV file
  if (file_chunk_id == DIST_FS_RIFF) {
    printf("RIFF chunk found\n");
    return DIST_FS_TYPE_WAV;
  } else {
    printf("Mismatch: Expected 0x%08X (%s), but got 0x%08X (%s)\n", DIST_FS_RIFF,
           hex_to_ascii(DIST_FS_RIFF).c_str(), file_chunk_id, ascii_chunk_id.c_str());
    return DIST_FS_TYPE_UNKNOWN;
  }
}

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
