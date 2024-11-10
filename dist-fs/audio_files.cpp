#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>

#include <cstdlib>
#include <stdio.h>

#include "audio_files.hpp"
#include "utils.hpp"

int get_wav_file(const char *file) {
  int rc = 0;

  return rc;
}

dist_fs_file_types_e get_file_type(const char *filename) {
  dist_fs_file_types_e file_type;

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

  LOG(INFO, "File size: %d bytes", audio_file_data.size());

  // save the file's first 4 bytes, this will determine what kind of file we want
  // to check for
  uint32_t file_chunk_id = (static_cast<uint8_t>(audio_file_data[0]) << 24) |
                           (static_cast<uint8_t>(audio_file_data[1]) << 16) |
                           (static_cast<uint8_t>(audio_file_data[2]) << 8) |
                           static_cast<uint8_t>(audio_file_data[3]);

  // first 4 bytes in ascii
  std::string ascii_chunk_id = hex_to_ascii(file_chunk_id);
  //printf("File Chunk ID: 0x%08X (%s)\n", file_chunk_id, ascii_chunk_id.c_str());
  LOG(INFO, "File Chunk ID: 0x%08X (%s)", file_chunk_id, ascii_chunk_id.c_str());


  // based on the first 4 bytes, lets switch case our way thru possible options
  switch (file_chunk_id) {
    case DIST_FS_RIFF:
      LOG(INFO, "RIFF chunk ID found");
      return DIST_FS_TYPE_WAV;

    case DIST_FS_FLAC:
      LOG(INFO, "fLaC chunk ID found");
      return DIST_FS_TYPE_FLAC;

    case DIST_FS_AIFF:
      LOG(INFO, "AIFF chunk ID found");
      return DIST_FS_TYPE_AIFF;

    case DIST_FS_MP3:
      LOG(INFO, "MP3 chunk ID found");
      return DIST_FS_TYPE_MP3;

    default:
      LOG(ERR, "Unknown file chunk ID: { hex:(0x%08X) ascii:(%s) }",
          file_chunk_id, ascii_chunk_id.c_str());

      return DIST_FS_TYPE_UNKNOWN;
  }
}
