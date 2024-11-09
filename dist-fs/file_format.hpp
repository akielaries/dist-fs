#pragma once

/** some definitions related to the wav file format */
#define DIST_FS_TYPE_SZ 4

#define DIST_FS_RIFF 0x52494646
#define DIST_FS_WAVE 0x57415645
#define DIST_FS_FMT  0x666d7420

/** @brief enumeration of file types */
typedef enum {
  DIST_FS_TYPE_UNKNOWN,
  DIST_FS_TYPE_WAV,
  DIST_FS_TYPE_FLAC,
  DIST_FS_TYPE_AIFF,
  DIST_FS_TYPE_MP3,

  DIST_FS_END,
  DIST_FS_NUM_TYPES = DIST_FS_END - 1,
} dist_fs_file_types_e;
