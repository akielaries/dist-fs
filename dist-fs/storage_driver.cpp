// an idea for this. the "files" should be encapsulated in a header of their
// own. this will contains stuff like file name, size, stamps, etc. when we
// eventually want to search for a file, we can use this defined header to
// basically "skip" thru the file system to "ls" our files

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <libgen.h>

#include "utils.hpp"
#include "audio_files.hpp"
#include "storage.hpp"

#define DEVICE_PATH  "/dev/sda"
#define PATTERN_SIZE 5
#define BLOCK_SIZE   512


/*TODO: I suspect some heavy optimizations will need to be done here */
int upload_file(const char *filename) {
  LOG(INFO, "Uploading file: %s", filename);

  // create some struct for file information here

  dist_fs_file_types_e file_type = get_file_type(filename);
  
  LOG(INFO, "Got file type: %d", file_type);


  LOG(INFO, "Creating FS header");
  LOG(INFO, "filename : hex:() ascii:(%s)", basename(const_cast<char *>(filename)));
  LOG(INFO, "");

  return 0;
}

int download_file(const char *filename) {
  LOG(INFO, "Downloading file: %s", filename);
  return 0;
}

int delete_file(const char *filename) {
  LOG(INFO, "Deleting file: %s", filename);
  return 0;
}

int list_files() {
  LOG(INFO, "Listing all files on the drive");
  return 0;
}

int ssd_read() {
  int rc = 0;

  return rc;
}

int ssd_write() {
  int rc = 0;

  return rc;
}

int ssd_echo(const unsigned char *pattern) {
  unsigned char read_buffer[DIST_FS_SSD_PATTERN_SZ];

  int fd = open(DEVICE_PATH, O_RDWR);
  if (fd == -1) {
    LOG(ERR, "Error opening device");
    return 1;
  }

  ssize_t written = write(fd, pattern, DIST_FS_SSD_PATTERN_SZ);
  if (written == -1) {
    LOG(ERR, "Error writing to device");
    close(fd);
    return 1;
  }

  LOG(INFO, "Wrote %zd bytes to %s", written, DEVICE_PATH);
  for (ssize_t i = 0; i < written; i++) {
    printf("0x%X ", pattern[i]);
  }
  printf("\n");

  if (lseek(fd, 0, SEEK_SET) == -1) {
    LOG(ERR, "Error seeking to start of device");
    close(fd);
    return 1;
  }

  ssize_t read_bytes = read(fd, read_buffer, DIST_FS_SSD_PATTERN_SZ);
  if (read_bytes == -1) {
    LOG(ERR, "Error reading from device");
    close(fd);
    return 1;
  } else {
    LOG(INFO, "Read %d bytes:", read_bytes);
    for (ssize_t i = 0; i < read_bytes; i++) {
      printf("0x%X ", read_buffer[i]);
    }
    printf("\n");
  }

  if (read_bytes == DIST_FS_SSD_PATTERN_SZ &&
      memcmp(pattern, read_buffer, DIST_FS_SSD_PATTERN_SZ) == 0) {
    LOG(INFO, "Echo test successful. Pattern matched");
  } else {
    LOG(INFO, "Echo test failed. Pattern did not match");
  }

  close(fd);
  return 0;
}
