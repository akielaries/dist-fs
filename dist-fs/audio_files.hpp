#pragma once

#include <cstdint>

/** some definitions related to the wav file format */
#define DIST_FS_TYPE_SZ 4
#define DIST_FS_ID_HEADER 16


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
  DIST_FS_TYPE_M4A,
  DIST_FS_TYPE_MP3,

  DIST_FS_END,
  DIST_FS_NUM_TYPES = DIST_FS_END - 1,
} dist_fs_file_types_e;

/** @brief structure for wav file information */
typedef struct {
  char  w_fileid[4];              /* chunk id 'RIFF'            */
  uint32_t w_chunksize;             /* chunk size                 */
  char  w_waveid[4];              /* wave chunk id 'WAVE'       */
  char  w_fmtid[4];               /* format chunk id 'fmt '     */
  uint32_t w_fmtchunksize;          /* format chunk size          */
  uint16_t  w_fmttag;               /* format tag, 1 for PCM      */
  uint16_t  w_nchannels;            /* number of channels         */
  uint32_t w_samplespersec;         /* sample rate in hz          */
  uint32_t w_navgbytespersec;       /* average bytes per second   */
  uint16_t  w_nblockalign;          /* number of bytes per sample */
  uint16_t  w_nbitspersample;       /* number of bits in a sample */
  char  w_datachunkid[4];         /* data chunk id 'data'       */
  uint32_t w_datachunksize;         /* length of data chunk       */
} dist_fs_wav_t;


dist_fs_file_types_e get_file_type(const char *filename);
