#pragma once

/** some definitions related to the wav file format */
#define DIST_FS_TYPE_SZ 4


/* defines for WAV files */
#define DIST_FS_RIFF 0x52494646
#define DIST_FS_WAVE 0x57415645
#define DIST_FS_FMT  0x666d7420

/* defines for FLAC files */
#define DIST_FS_FLAC 0x664C6143

/* defines for MP3 files */
#define DIST_FS_MP3 0x49443304

/* defines for AIFF files */
#define DIST_FS_AIFF 0x464f524d

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


dist_fs_file_types_e get_file_type(const char *filename);
