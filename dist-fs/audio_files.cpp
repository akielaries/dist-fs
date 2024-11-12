#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>

#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <sys/stat.h>

#include <libgen.h>

#include "audio_files.hpp"
#include "utils.hpp"

// Helper function to print section details
void print_chunk_info(const std::string &label, uint32_t value) {
  std::cout << label << ": 0x" << std::hex << std::setw(8) << std::setfill('0')
            << value << std::dec << " (" << value << ")\n";
}

int get_wav_file(std::ifstream &file) {
  // make sure we're at the start of the file stream
  file.seekg(0);

  // Read the RIFF header (first 12 bytes)
  // RIFF header is within the first 12 bytes, lets make sure
  char riff_header[DIST_FS_ID_HEADER];
  file.read(riff_header, DIST_FS_ID_HEADER);
  if (file.gcount() < DIST_FS_ID_HEADER ||
      std::string(riff_header, 4) != "RIFF") {
    LOG(ERR, "Missing RIFF header");
    return -1;
  }

  uint32_t chunk_size = *reinterpret_cast<uint32_t *>(&riff_header[4]);
  if (std::string(riff_header + 8, 4) != "WAVE") {
    LOG(ERR, "Missing WAVE format identifier");
    return -1;
  }

  LOG(INFO, "Chunk Size : %d", chunk_size);

  // read the chunks...
  while (file) {
    char subchunk_id[4];
    file.read(subchunk_id, 4);
    if (file.gcount() < 4)
      break;


    uint32_t subchunk_size;
    file.read(reinterpret_cast<char *>(&subchunk_size), 4);
    if (file.gcount() < 4)
      break;

    std::string chunk(subchunk_id, 4);
    // if we encounter the 'fmt ' chunk
    // if (chunk == hex_to_ascii((uint32_t)DIST_FS_FMT)) {
    if (chunk == "fmt ") {
      uint16_t audio_format, num_channels, block_align, bits_per_sample;
      uint32_t sample_rate, byte_rate;
      file.read(reinterpret_cast<char *>(&audio_format), sizeof(audio_format));
      file.read(reinterpret_cast<char *>(&num_channels), sizeof(num_channels));
      file.read(reinterpret_cast<char *>(&sample_rate), sizeof(sample_rate));
      file.read(reinterpret_cast<char *>(&byte_rate), sizeof(byte_rate));
      file.read(reinterpret_cast<char *>(&block_align), sizeof(block_align));
      file.read(reinterpret_cast<char *>(&bits_per_sample),
                sizeof(bits_per_sample));

      LOG(INFO, "Format subchunk ('%s'): ", chunk.c_str());
      LOG(INFO, "  Audio Format: %d", audio_format);
      LOG(INFO, "  Channels: %d", num_channels);
      LOG(INFO, "  Sample Rate: %d", sample_rate);
      LOG(INFO, "  Byte Rate: %d", byte_rate);
      LOG(INFO, "  Block Align: %d", block_align);
      LOG(INFO, "  Bits per Sample: %d", bits_per_sample);
    }

    else if (chunk == "data") {
      LOG(INFO, "Data subchunk ('%s')", chunk.c_str());
      LOG(INFO, "  Data Size:  %d", subchunk_size);
      // skip data section. we don't need to do anything with this
      file.seekg(subchunk_size, std::ios::cur);
    }

    else {
      // handle optional chunks like INFO and JUNK
      LOG(INFO,
          "Optional subchunk ('%s') with size: %d",
          chunk.c_str(),
          subchunk_size);
      // skip
      file.seekg(subchunk_size, std::ios::cur);
    }
  }

  return 0;
}

int get_file_info(file_info_t &file_info, const char *filename) {
  file_info.name = basename(const_cast<char *>(filename));
  LOG(INFO, "Checking format of file: (%s)", file_info.name);
  int rc = 0;

  struct stat file_stat;
  if (stat(filename, &file_stat) != 0) {
      LOG(ERR, "Could not retrieve file information for %s\n", filename);
      return DIST_FS_TYPE_FAILURE;
  }

  file_info.timestamp = file_stat.st_mtime;

  // convert to string and remove the trailing newline
  std::string time_str = std::ctime(&file_info.timestamp);
  // remove new line
  time_str = time_str.substr(0, time_str.length() - 1);

  // read in the audio file
  std::ifstream audio_file(filename, std::ios::binary);
  if (!audio_file) {
    LOG(ERR, "No such file %s\n", filename);
    return DIST_FS_TYPE_FAILURE;
  }

  // move to the end of the stream to get the size, then move the pointer back
  // to start of the stream
  audio_file.seekg(0, std::ios::end);
  std::streamsize file_size = audio_file.tellg();
  file_info.size = file_size;
  // reset pointer to start of file
  audio_file.seekg(0, std::ios::beg);
  LOG(INFO, "File size: %ld bytes", file_size);

  // file header, first 16 bytes of the file
  std::array<char, DIST_FS_ID_HEADER> header = {0};
  audio_file.read(header.data(), DIST_FS_ID_HEADER);

  if (audio_file.gcount() < DIST_FS_ID_HEADER) {
    LOG(ERR, "File is too small to contain a valid header\n");
    return DIST_FS_TYPE_FAILURE;
  }

  // extract identifier to two 32 bit variables
  uint32_t file_chunk_id_1 = (static_cast<uint8_t>(header[0]) << 24) |
                             (static_cast<uint8_t>(header[1]) << 16) |
                             (static_cast<uint8_t>(header[2]) << 8) |
                             static_cast<uint8_t>(header[3]);
  uint32_t file_chunk_id_2 = (static_cast<uint8_t>(header[4]) << 24) |
                             (static_cast<uint8_t>(header[5]) << 16) |
                             (static_cast<uint8_t>(header[6]) << 8) |
                             static_cast<uint8_t>(header[7]);

  // combine chunk IDs into a single 64 bit ID
  uint64_t file_chunk_id =
    (static_cast<uint64_t>(file_chunk_id_1) << 32) | file_chunk_id_2;

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
      // check if this is a valid wav file
      rc = get_wav_file(audio_file);
      if (rc != 0) {
        return DIST_FS_TYPE_UNKNOWN;
      }

      file_info.type = DIST_FS_TYPE_WAV;
      return rc;

    case DIST_FS_FLAC:
      LOG(INFO, "fLaC chunk ID found");
      file_info.type = DIST_FS_TYPE_FLAC;
      return rc;

    case DIST_FS_AIFF:
      LOG(INFO, "AIFF chunk ID found");
      file_info.type = DIST_FS_TYPE_AIFF;
      return rc;

    case DIST_FS_MP3:
      LOG(INFO, "MP3 chunk ID found");
      file_info.type = DIST_FS_TYPE_MP3;
      return rc;

    case DIST_FS_M4A_HEADER:
      LOG(INFO, "M4A chunk ID found");
      file_info.type = DIST_FS_TYPE_M4A;
      return rc;

    default:
      LOG(ERR,
          "Unknown file chunk ID: { hex:(0x%08X) ascii:(%s) }",
          file_chunk_id,
          ascii_chunk_id.c_str());

      return DIST_FS_TYPE_UNKNOWN;
  }
}
