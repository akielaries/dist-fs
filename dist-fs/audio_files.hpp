#pragma once

#include <cstdint>
#include <ctime>

/** some definitions related to the wav file format */
#define DIST_FS_TYPE_SZ   4
#define DIST_FS_ID_HEADER 16


/* defines for WAV files */
#define DIST_FS_RIFF 0x52494646 // ASCII: (RIFF)
#define DIST_FS_WAVE 0x57415645 // ASCII: (WAVE)
#define DIST_FS_FMT  0x666d7420 // ASCII: (fmt )

/* defines for FLAC files */
#define DIST_FS_FLAC 0x664C6143 // ASCII: (fLaC)

/* defines for MP3 files */
#define DIST_FS_MP3 0x49443304 // ASCII: (ID3 )

/* defines for AIFF files */
#define DIST_FS_AIFF 0x464f524d // ASCII: (FORM)

/* defines for M4A files */
#define DIST_FS_M4A_HEADER 0x0000001c
#define DIST_FS_M4A        0x4d344120 // ASCII: (M4A )


/** @brief enumeration of error codes */
typedef enum {
  DIST_FS_TYPE_FAILURE = -1,
  DIST_FS_TYPE_UNKNOWN = -2,
} dist_fs_error_codes_e;


/** @brief enumeration of file types */
typedef enum {
  DIST_FS_TYPE_WAV  = 0,
  DIST_FS_TYPE_FLAC = 1,
  DIST_FS_TYPE_AIFF = 2,
  DIST_FS_TYPE_M4A  = 3,
  DIST_FS_TYPE_MP3  = 4,
  // DIST_FS_TYPE_FOLDER = 5,
  /*add more files here*/
  DIST_FS_END,
  DIST_FS_NUM_TYPES = DIST_FS_END - 1,
} dist_fs_file_types_e;

/** @brief structure for wav file information */
typedef struct {
  char w_fileid[4];           /* chunk id 'RIFF'            */
  uint32_t w_chunksize;       /* chunk size                 */
  char w_waveid[4];           /* wave chunk id 'WAVE'       */
  char w_fmtid[4];            /* format chunk id 'fmt '     */
  uint32_t w_fmtchunksize;    /* format chunk size          */
  uint16_t w_fmttag;          /* format tag, 1 for PCM      */
  uint16_t w_nchannels;       /* number of channels         */
  uint32_t w_samplespersec;   /* sample rate in hz          */
  uint32_t w_navgbytespersec; /* average bytes per second   */
  uint16_t w_nblockalign;     /* number of bytes per sample */
  uint16_t w_nbitspersample;  /* number of bits in a sample */
  char w_datachunkid[4];      /* data chunk id 'data'       */
  uint32_t w_datachunksize;   /* length of data chunk       */
} dist_fs_wav_t;

/** @brief  */
typedef struct {
  char *name;                // file name
  uint64_t size;             // file size in bytes
  dist_fs_file_types_e type; // file type
  off_t offset;              // file offset in the drive
  std::time_t timestamp;     // file timestamp
} file_info_t;

int get_file_info(file_info_t &file_info, const char *filename);
