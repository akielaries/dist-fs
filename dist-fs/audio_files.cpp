#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>

#include <cstdlib>
#include <stdio.h>

#include <libgen.h>

#include "audio_files.hpp"
#include "utils.hpp"

int get_wav_file(const char *file) {
  int rc = 0;

  return rc;
}

dist_fs_file_types_e get_file_type(const char *filename) {
  LOG(INFO,
      "Checking file type of file: (%s)",
      basename(const_cast<char *>(filename)));
  dist_fs_file_types_e file_type;

  // read in the audio file
  std::ifstream audio_file(filename, std::ios::binary);
  if (!audio_file) {
    LOG(ERR, "No such file %s\n", filename);
    return DIST_FS_TYPE_UNKNOWN;
  }

  // move to the end of the stream to get the size, then move the pointer back
  // to start of the stream
  audio_file.seekg(0, std::ios::end);
  std::streamsize file_size = audio_file.tellg();
  // reset pointer to start of file
  audio_file.seekg(0, std::ios::beg);
  LOG(INFO, "File size: %ld bytes", file_size);

  //char header[DIST_FS_ID_HEADER] = { 0 };

  std::array<char, DIST_FS_ID_HEADER> header = { 0 };

  // snag first few bytes of the file header, enough to identify what we're working with
  audio_file.read(header.data(), DIST_FS_ID_HEADER);

  if (audio_file.gcount() < DIST_FS_ID_HEADER) {
    LOG(ERR, "File is too small to contain a valid header\n");
    return DIST_FS_TYPE_UNKNOWN;
  }

  // extract identifier to a single variable
    // Extract the first 8 bytes into two 32-bit chunks (lower 4 bytes and upper 4 bytes)
    uint32_t file_chunk_id_1 = (static_cast<uint8_t>(header[0]) << 24) |
                               (static_cast<uint8_t>(header[1]) << 16) |
                               (static_cast<uint8_t>(header[2]) << 8) |
                               static_cast<uint8_t>(header[3]);

    uint32_t file_chunk_id_2 = (static_cast<uint8_t>(header[4]) << 24) |
                               (static_cast<uint8_t>(header[5]) << 16) |
                               (static_cast<uint8_t>(header[6]) << 8) |
                               static_cast<uint8_t>(header[7]);

    // Combine them into one 64-bit value if needed (optional, if you want to use a single identifier)
    uint64_t file_chunk_id = (static_cast<uint64_t>(file_chunk_id_1) << 32) | file_chunk_id_2;

  // first 4 bytes in ascii
  std::string ascii_chunk_id = hex_to_ascii(header);
  LOG(INFO,
      "File Chunk ID: 0x%16lX (%s)",
      file_chunk_id,
      ascii_chunk_id.c_str());

  // based on the first 4 bytes, lets switch case our way thru possible options
  switch (file_chunk_id_1) {
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
      LOG(ERR,
          "Unknown file chunk ID: { hex:(0x%08X) ascii:(%s) }",
          file_chunk_id,
          ascii_chunk_id.c_str());

      return DIST_FS_TYPE_UNKNOWN;
  }
}
