// an idea for this. the "files" should be encapsulated in a header of their
// own. this will contains stuff like file name, size, stamps, etc. when we
// eventually want to search for a file, we can use this defined header to
// basically "skip" thru the file system to "ls" our files

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <vector>
#include <cstring>

#include "utils.hpp"
#include "audio_files.hpp"
#include "storage.hpp"

#define DEVICE_PATH "/dev/sda"

// metadata table starts at the very beginning of the drive
const off_t METADATA_TABLE_OFFSET = 0;
// max files I want to track for now...
const size_t MAX_FILES = 1024;


struct FileMetadata {
    char filename[256];  // File name
    off_t start_offset;  // Start location on SSD
    size_t size;         // File size in bytes
};


std::vector<FileMetadata> read_metadata_table(int fd) {
  LOG(INFO, "Reading SSD metadata table");
    std::vector<FileMetadata> metadata_table;

    if (lseek(fd, METADATA_TABLE_OFFSET, SEEK_SET) == -1) {
        LOG(ERR, "Error seeking to metadata table");
        return metadata_table;
    }

    FileMetadata entry;
    for (size_t i = 0; i < MAX_FILES; ++i) {
        ssize_t read_bytes = read(fd, &entry, sizeof(FileMetadata));
        if (read_bytes != sizeof(FileMetadata)) break;  // End of table or error
        if (entry.start_offset == 0 && entry.size == 0) break;  // Empty entry
        metadata_table.push_back(entry);
    }

    return metadata_table;
}

bool write_metadata_entry(int fd, const FileMetadata& entry) {
  LOG(INFO, "Writing metadata entry");
    if (lseek(fd, METADATA_TABLE_OFFSET, SEEK_END) == -1) {
        LOG(ERR, "Error seeking to end of metadata table");
        return false;
    }

    ssize_t written = write(fd, &entry, sizeof(FileMetadata));
    if (written != sizeof(FileMetadata)) {
        LOG(ERR, "Error writing metadata entry");
        return false;
    }

    return true;
}

off_t find_next_free_offset(const std::vector<FileMetadata>& metadata_table) {
  LOG(INFO, "Finding next free offset for file storage");
    const off_t METADATA_SIZE = MAX_FILES * sizeof(FileMetadata);
    LOG(INFO, "Metadata Size        : %d", METADATA_SIZE);
    LOG(INFO, "Max files            : %d", MAX_FILES);
    LOG(INFO, "sizeof(FileMetadata) : %d", sizeof(FileMetadata));
    if (metadata_table.empty()) {
        // No files, start right after metadata
        LOG(INFO, "No files, starting right after metadata section");
        return METADATA_SIZE;
    }

    // Find the highest end offset among all files
    off_t max_end_offset = METADATA_SIZE;  // Start after metadata
    for (const auto& entry : metadata_table) {
        off_t end_offset = entry.start_offset + entry.size;
        if (end_offset > max_end_offset) max_end_offset = end_offset;
    }

    return max_end_offset;
}

/*TODO: I suspect some heavy optimizations will need to be done here */
int upload_file(const char *filename) {
  LOG(INFO, "Uploading file: %s", filename);
  int rc = 0;
  // create some struct for file information here
  file_info_t file_info = {0};

  rc = get_file_info(file_info, filename);
  // TODO, check rc of get_file_info

  // open SSD
  int ssd_fd = open(DEVICE_PATH, O_RDWR);
  if (ssd_fd == -1) {
    LOG(ERR, "Error opening SSD");
      return 1;
  }


  LOG(INFO, "Creating FS header");
  LOG(INFO, " start bytes: 0x%8X", DIST_FS_SSD_HEADER);
  LOG(INFO, " file type: %d", file_info.type);
  LOG(INFO, " filename : hex:() ascii:(%s)", file_info.name);
  LOG(INFO,
      " file size: %db | %dkb | %dmb | %dgb",
      file_info.size,
      (file_info.size / 1024),
      (file_info.size / 1024) / 1024,
      ((file_info.size / 1024) / 1024) / 1024);
  LOG(INFO, " file timestamp: %s", std::ctime(&file_info.timestamp));

  std::vector<FileMetadata> metadata_table = read_metadata_table(ssd_fd);
  off_t next_offset = find_next_free_offset(metadata_table);
  LOG(INFO, "Next free offset: 0x%08lX/%d", next_offset, next_offset);



  LOG(INFO, "Writing FS header");
  // write the file system header, start bytes, file type, filename, file
  // size(bytes), timestamp write the audio file Open the file to upload
  
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

int ssd_read(unsigned char *buffer, size_t size, off_t offset) {
  int fd = open(DEVICE_PATH, O_RDONLY);
  if (fd == -1) {
    std::cerr << "Error opening device: " << strerror(errno) << "\n";
    return 1;
  }

  if (lseek(fd, offset, SEEK_SET) == -1) {
    std::cerr << "Error seeking to offset " << offset << ": " << strerror(errno)
              << "\n";
    close(fd);
    return 1;
  }

  ssize_t read_bytes = read(fd, buffer, size);
  if (read_bytes == -1) {
    std::cerr << "Error reading from device: " << strerror(errno) << "\n";
    close(fd);
    return 1;
  }

  std::cout << "Read " << read_bytes << " bytes from " << DEVICE_PATH
            << " at offset " << offset << "\n";
  for (ssize_t i = 0; i < read_bytes; ++i) {
    printf("0x%X ", buffer[i]);
  }
  printf("\n");

  close(fd);
  return 0;
}

int ssd_write(const unsigned char *buffer, size_t size, off_t offset) {
  int fd = open(DEVICE_PATH, O_RDWR);
  if (fd == -1) {
    std::cerr << "Error opening device: " << strerror(errno) << "\n";
    return 1;
  }

  if (lseek(fd, offset, SEEK_SET) == -1) {
    std::cerr << "Error seeking to offset " << offset << ": " << strerror(errno)
              << "\n";
    close(fd);
    return 1;
  }

  ssize_t written = write(fd, buffer, size);
  if (written == -1) {
    std::cerr << "Error writing to device: " << strerror(errno) << "\n";
    close(fd);
    return 1;
  }

  std::cout << "Wrote " << written << " bytes to " << DEVICE_PATH
            << " at offset " << offset << "\n";
  close(fd);
  return 0;
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
