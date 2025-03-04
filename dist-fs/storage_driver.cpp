#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <endian.h>
#include <sys/stat.h>

#include <vector>
#include <cstring>

#include <chrono>

#include "utils.hpp"
#include "audio_files.hpp"
#include "storage.hpp"


int get_time_info(storage_metadata_t *md_table) {
  if (!md_table) {
    fprintf(stderr, "Error: md_table pointer is NULL\n");
    return -1;
  }

  struct stat file_stat;
  if (stat(md_table->filename, &file_stat) == -1) {
    perror("stat");
    return -1;
  }

  // populate file_times_t
  md_table->file_time.last_modified = file_stat.st_mtime;
  md_table->file_time.last_accessed = file_stat.st_atime;

#ifdef __USE_STATX
  // if the system supports statx, attempt to retrieve creation time
  struct statx file_statx;
  if (statx(AT_FDCWD,
            md_table->filename,
            AT_STATX_SYNC_AS_STAT,
            STATX_BTIME,
            &file_statx) == 0) {
    md_table->file_time.created = file_statx.stx_btime.tv_sec;
  } else {
    perror("statx");
    md_table->file_time.created = 0;
  }
#else
  // use ctime as a fallback for systems without statx
  md_table->file_time.created = file_stat.st_ctime;
#endif

  // set uploaded timestamp to current time
  struct timespec now;
  if (clock_gettime(CLOCK_REALTIME, &now) == -1) {
    perror("clock_gettime");
    return -1;
  }
  md_table->file_time.uploaded = now.tv_sec;

  LOG(INFO, "Time info for file {%s}", md_table->filename);
  char time_buffer[64];

  struct tm *tm_info = localtime(&md_table->file_time.last_modified);
  strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", tm_info);
  LOG(INFO, "  - last modified: %s", time_buffer);

  tm_info = localtime(&md_table->file_time.last_accessed);
  strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", tm_info);
  LOG(INFO, "  - last accessed: %s", time_buffer);

  tm_info = localtime(&md_table->file_time.created);
  strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", tm_info);
  LOG(INFO, "  - created: %s", time_buffer);

  tm_info = localtime(&md_table->file_time.uploaded);
  strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", tm_info);
  LOG(INFO, "  - uploaded: %s", time_buffer);

  return 0; // Success
}

// metadata table operations
/*****************************************************************************/
std::vector<storage_metadata_t> md_table_read(int ssd_fd) {
  LOG(INFO, "Reading SSD metadata table");
  std::vector<storage_metadata_t> md_table;

  // seek from beginning of file
  if (lseek(ssd_fd, 0, SEEK_SET) == -1) {
    LOG(ERR, "Failed to seek to metadata table");
    return md_table;
  }

  // read the metadata section
  char buffer[METADATA_TABLE_SZ] = {0};
  ssize_t bytes_read             = read(ssd_fd, buffer, METADATA_TABLE_SZ);
  if (bytes_read <= 0) {
    LOG(INFO, "No metadata found. Initializing empty table");
    return md_table;
  }

  // parse metadata entries
  storage_metadata_t *entries = reinterpret_cast<storage_metadata_t *>(buffer);
  size_t num_entries          = bytes_read / sizeof(storage_metadata_t);

  for (size_t i = 0; i < num_entries; ++i) {
    if (entries[i].start_offset != 0) { // valid entry check
      md_table.push_back(entries[i]);
    }
  }

  return md_table;
}

bool md_table_write(int ssd_fd, storage_metadata_t &entry, size_t index) {
  off_t entry_offset =
    METADATA_TABLE_OFFSET + (index * sizeof(storage_metadata_t));
  LOG(INFO, "Writing metadata entry at offset: 0x%08lX", entry_offset);

  // seek to the metadata entry offset
  if (lseek(ssd_fd, entry_offset, SEEK_SET) == -1) {
    LOG(ERR, "Failed to seek to metadata offset: 0x%08lX", entry_offset);
    return false;
  }

  // write metadata entry in the table
  ssize_t written = write(ssd_fd, &entry, sizeof(entry));
  if (written != sizeof(entry)) {
    LOG(ERR,
        "Error writing metadata entry. Expected %lu bytes, wrote %ld bytes",
        sizeof(entry),
        written);
    return false;
  }

  LOG(INFO,
      "Successfully wrote metadata entry at offset: 0x%08lX",
      entry_offset);
  return true;
}

int md_table_print(const std::vector<storage_metadata_t> &md_table) {
  // TODO get rid of all the static definitions of line widths
  // determine maximum column widths dynamically
  size_t max_filename_length = 0;
  size_t max_index_length    = 0 + 5; // index is 5 chars long
  size_t max_offset_length   = 0;
  size_t max_size_length     = 0;
  size_t max_kb_length       = 0;
  size_t max_mb_length       = 0;

  for (const auto &entry : md_table) {
    max_filename_length = std::max(max_filename_length, strlen(entry.filename));
    max_index_length =
      std::max(max_index_length, std::to_string(entry.index).size());
    max_offset_length =
      std::max(max_offset_length, std::to_string(entry.start_offset).size());
    max_size_length =
      std::max(max_size_length, std::to_string(entry.size).size());
    max_kb_length =
      std::max(max_kb_length, std::to_string(entry.size / 1024).size());
    max_mb_length =
      std::max(max_mb_length, std::to_string(entry.size / 1024 / 1024).size());
  }

  // cast size_t to int for setw
  int filename_width = static_cast<int>(max_filename_length);
  int index_width    = static_cast<int>(max_index_length);
  int offset_width   = static_cast<int>(max_offset_length);
  int size_width     = static_cast<int>(max_size_length);
  int size_kb_width  = static_cast<int>(max_kb_length);
  int size_mb_width  = static_cast<int>(max_mb_length);

  // print the header row with dynamic column widths
  std::cout << std::left << std::setw(filename_width) << "Name"
            << " | " << std::setw(index_width) << "Index"
            << " | " << std::setw(offset_width) << "Offset"
            << " | " << std::setw(size_width) << "bytes"
            << " | " << std::setw(size_kb_width) << "kb"
            << " | " << std::setw(size_mb_width) << "mb"
            << " | " << std::setw(20) << "Modified"
            << " | " << std::setw(20) << "Accessed"
            << " | " << std::setw(20) << "Created"
            << " | " << std::setw(20) << "Uploaded" << std::endl;

  // print the separator line
  std::cout << std::string(filename_width + index_width + offset_width +
                             size_width + size_kb_width + size_mb_width + 100,
                           '-')
            << std::endl;

  // print the data rows with dynamic column widths
  for (const auto &entry : md_table) {
    // format timestamps
    auto format_time = [](std::time_t t) -> std::string {
      std::ostringstream oss;
      std::tm *tm_info = std::localtime(&t);
      if (tm_info) {
        oss << std::put_time(tm_info, "%Y-%m-%d %H:%M:%S");
      } else {
        oss << "N/A"; // handle invalid times
      }
      return oss.str();
    };

    std::cout << std::left << std::setw(filename_width) << entry.filename
              << " | " << std::right << std::setw(index_width + 1)
              << entry.index << "|" << std::right << std::setw(offset_width + 1)
              << std::hex << entry.start_offset << " | " << std::right
              << std::setw(size_width) << std::dec << entry.size << " | "
              << std::right << std::setw(size_kb_width) << std::dec
              << entry.size / 1024 << " | " << std::right
              << std::setw(size_mb_width) << std::dec
              << entry.size / 1024 / 1024 << " | " << std::setw(20)
              << format_time(entry.file_time.last_modified) << " | "
              << std::setw(20) << format_time(entry.file_time.last_accessed)
              << " | " << std::setw(20) << format_time(entry.file_time.created)
              << " | " << std::setw(20) << format_time(entry.file_time.uploaded)
              << std::endl;
  }

  return 0;
}

off_t md_table_find_offset(const std::vector<storage_metadata_t> &md_table) {
  LOG(INFO, "Finding next free offset for file storage");
  const off_t METADATA_SIZE = MAX_FILES * sizeof(storage_metadata_t);
  LOG(INFO, "Metadata Size          : %d", METADATA_SIZE);
  LOG(INFO, "Max files              : %d", MAX_FILES);
  LOG(INFO, "sizeof(storage_metadata_t) : %d", sizeof(storage_metadata_t));
  if (md_table.empty()) {
    LOG(INFO, "No files, starting right after metadata section");
    return METADATA_SIZE;
  }

  // start searching after the metadata section
  off_t max_end_offset = METADATA_SIZE;
  // find the highest end offset among all files
  for (const auto &entry : md_table) {
    off_t end_offset = entry.start_offset + entry.size;
    if (end_offset > max_end_offset)
      max_end_offset = end_offset;
  }
  return max_end_offset;
}

int update_md_table(storage_metadata_t *md_table,
                    file_info_t &file_info,
                    int ssd_fd) {

  std::vector<storage_metadata_t> md_vector = md_table_read(ssd_fd);
  // index in the metadata table
  // size_t index = md_vector.size();
  md_table->index = md_vector.size();


  // get file information
  int rc = get_time_info(md_table);
  if (rc != 0) {
    LOG(ERR, "Error getting time information : {%d}", rc);
    return rc;
  }


  md_table->start_offset = file_info.offset;
  md_table->size         = file_info.size;

  LOG(INFO, "Updating metadata table with entry for file : %s", file_info.name);
  LOG(INFO, " start_offset : 0x%X", md_table->start_offset);
  LOG(INFO, " size         : %d bytes", md_table->size);

  LOG(INFO, "Metadata table size : %d", md_table->index);
  if (!md_table_write(ssd_fd, *md_table, md_table->index)) {
    LOG(ERR, "Failed to write metadata entry for file: %s", md_table->filename);
    return 1;
  }

  return 0;
}


int write_fs_header(int ssd_fd, off_t offset, const file_info_t &file_info) {
  LOG(INFO, "Creating FS header");
  LOG(INFO, " start bytes: 0x%8X", DIST_FS_SSD_HEADER);
  LOG(INFO, " filename : hex:() ascii:(%s)", file_info.name);
  LOG(INFO,
      " file size: %db | %dkb | %dmb | %dgb",
      file_info.size,
      (file_info.size / 1024),
      (file_info.size / 1024) / 1024,
      ((file_info.size / 1024) / 1024) / 1024);
  LOG(INFO, " file type: %d", file_info.type);
  LOG(INFO, " file offset: %d", file_info.offset);
  LOG(INFO,
      " file timestamp: %s",
      strip_newline(std::ctime(&file_info.timestamp)).c_str());


  // TODO/BUG: endianness matters for the header, does it for the rest of the
  // data?? unit tests should expose if this is the case as it verifies the file
  // uploaded vs the one downloaded
  uint32_t header_be = htobe32(DIST_FS_SSD_HEADER);
  lseek(ssd_fd, offset, SEEK_SET);
  if (write(ssd_fd, &header_be, sizeof(header_be)) != sizeof(header_be)) {
    LOG(ERR, "Failed to write FS header");
    return 1;
  }
  lseek(ssd_fd, offset + 4, SEEK_SET);
  LOG(INFO, "Writing FS header at offset: 0x%08lX", offset);
  if (write(ssd_fd, &file_info, sizeof(file_info)) != sizeof(file_info)) {
    LOG(ERR, "Failed to write file info");
    return 1;
  }
  return 0;
}


// hard drive operations
/*****************************************************************************/
bool is_drive_provisioned(config_context_t cfg_ctx) {
  (void)cfg_ctx;

  // TODO check if a hard drive is provisioned or not

  return true;
}

int drive_provision(config_context_t cfg_ctx) {
  (void)cfg_ctx;
  // TODO this should provision the first N bytes of the drive with some
  // information. there should always be a check for some magic numbers to
  // ensure the drive we are working with is indeed the right one and produce
  // some sort of warning/error on failure

  int rc = 0;

  // check if provisioned already
  // is_provisioned = is_drive_provisioned(cfg_ctx);
  // if (is_provisioned) {
  //   LOG(INFO, "Drive %s is already provisioned for dist-fs",
  //              cfg_ctx.drive_full_path);
  //   return -1;
  // }

  return rc;
}

int drive_info(config_context_t cfg_ctx) {
  (void)cfg_ctx;
  LOG(INFO, "Getting drive information");
  // TODO get the total size of the drive and use the metadata table to get
  // how much of it is actually in use. should be fine with lseek?
  int rc = 0;

  // any way to grab manufacturer info? other hardware/device info?

  return rc;
}

int initialize_ssd(config_context_t cfg_ctx, int &ssd_fd, off_t &next_offset) {
  ssd_fd = open(cfg_ctx.drive_full_path, O_RDWR);
  if (ssd_fd == -1) {
    LOG(ERR, "Error opening SSD");
    return 1;
  }
  std::vector<storage_metadata_t> md_table = md_table_read(ssd_fd);
  next_offset                              = md_table_find_offset(md_table);
  LOG(INFO, "Next free offset: 0x%08lX/%d", next_offset, next_offset);
  return 0;
}


// file operations
/*****************************************************************************/
int transfer_file_data(int file_fd, int ssd_fd, off_t offset) {
  LOG(INFO, "Writing file data to SSD at offset: 0x%08lX", offset);

  // 4kb buffer
  char buffer[4096];
  ssize_t bytes_read, bytes_written, total_bytes_written;

  lseek(ssd_fd, offset, SEEK_SET);

  auto start_time = std::chrono::high_resolution_clock::now();

  while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0) {
    bytes_written = write(ssd_fd, buffer, bytes_read);
    if (bytes_written != bytes_read) {
      LOG(ERR, "Failed to write file data");
      return 1;
    }
    total_bytes_written += bytes_written;
  }
  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end_time - start_time;

  if (bytes_read == -1) {
    LOG(ERR, "Error reading file");
    return 1;
  }

  // upload speed (bytes per second)
  double upload_speed =
    static_cast<double>(total_bytes_written) / duration.count();
  LOG(INFO, "Total bytes written: %ld", total_bytes_written);
  LOG(INFO, "Upload time: %.2f seconds", duration.count());
  LOG(INFO,
      "Upload speed: %.2f kbps | %.2f mbps",
      upload_speed / 1024,
      upload_speed / 1024 / 1024);

  return 0;
}

/*TODO: I suspect some heavy optimizations will need to be done here */
int upload_file(config_context_t cfg_ctx, const char *filename) {
  LOG(INFO, "Uploading file: %s", filename);
  int rc = 0;

  // create some struct for file information here
  // INQUIRE: I should look into why ={0} creates a warning but ={} doesn't
  file_info_t file_info = {};

  // TODO: somewhere here, we should check if filename is a directory or a
  // single file. if a single file, we can proceed as normal, if a directory,
  // the logic in this function should really be ran for each file
  // TODO: I want to create a tree to keep track of the folder (root node) and
  // child/parent nodes based on what's inside
  /* below:
    stems/                                  root node
    └── wavs/                               grandparent node
        ├── drums/                          parent node of subtree A
        │   ├── track_hats.wav              child node
        │   ├── track_shaker.wav            child node
        │   ├── track_kicks.wav             child node
        │   ├── track_snare_1.wav           child node
        │   ├── track_snare_2.wav           child node
        │   └── track_clap.wav              child node
        └── keyboards/                      parent node of subtree B
            ├── juno_bass.wav               child node
            ├── juno_lead.wav               child node
            ├── moog_bass.wav               child node
            ├── moog_pad.wav                child node
            ├── steinway_piano_part1.wav    child node
            └── steinway_piano_part2.wav    child node
  */

  // get file info. for now this is only available for audio files
  rc = get_file_info(file_info, filename);
  if (rc != 0) {
    LOG(ERR, "Failed to retrieve file info for: %s", filename);
    return 1;
  }


  // open SSD + read from the metadata table to get the next available
  // offset in the FS
  int ssd_fd;
  off_t next_offset;
  if (initialize_ssd(cfg_ctx, ssd_fd, next_offset)) {
    return 1;
  }
  file_info.offset = next_offset;

  // read from the metadata table to get the next available offset in the FS
  if (write_fs_header(ssd_fd, next_offset, file_info)) {
    close(ssd_fd);
    return 1;
  }

  // open the file for upload
  int file_fd = open(filename, O_RDONLY);
  if (file_fd == -1) {
    LOG(ERR, "Error opening file: %s", filename);
    close(ssd_fd);
    return 1;
  }

  if (transfer_file_data(file_fd,
                         ssd_fd,
                         next_offset + sizeof(uint32_t) + sizeof(file_info))) {
    close(file_fd);
    close(ssd_fd);
    return 1;
  }

  storage_metadata_t md_table;
  strncpy(md_table.filename, filename, sizeof(md_table.filename) - 1);

  // update the metadata table with a new entry
  if (update_md_table(&md_table, file_info, ssd_fd)) {
    close(file_fd);
    close(ssd_fd);
    return 1;
  }

  // close both file descriptors
  close(file_fd);
  close(ssd_fd);

  return 0;
}

int download_file(config_context_t cfg_ctx, const char *filename) {
  LOG(INFO, "Downloading file: %s", filename);

  int ssd_fd = open(cfg_ctx.drive_full_path, O_RDONLY);
  if (ssd_fd < 0) {
    LOG(ERR, "Failed to open SSD device: %s", cfg_ctx.drive_full_path);
    return -1;
  }

  std::vector<storage_metadata_t> md_table = md_table_read(ssd_fd);
  if (md_table.empty()) {
    LOG(ERR, "Failed to read SSD metadata table.");
    close(ssd_fd);
    return -1;
  }

  auto it = std::find_if(md_table.begin(),
                         md_table.end(),
                         [filename](const storage_metadata_t &entry) {
                           return strcmp(entry.filename, filename) == 0;
                         });

  if (it == md_table.end()) {
    LOG(ERR, "File '%s' not found on SSD.", filename);
    close(ssd_fd);
    return -1;
  }

  off_t start_offset   = it->start_offset;
  size_t file_size     = it->size;
  const char *basename = strrchr(filename, '/');
  basename             = (basename) ? basename + 1 : filename;

  FILE *local_file = fopen(basename, "wb");
  if (!local_file) {
    LOG(ERR, "Failed to create local file: %s", basename);
    close(ssd_fd);
    return -1;
  }

  char buffer[4096];
  ssize_t bytes_read;
  off_t file_offset =
    start_offset + sizeof(DIST_FS_SSD_HEADER) + sizeof(file_info_t);

  while (file_size > 0) {
    size_t to_read = std::min(file_size, sizeof(buffer));
    bytes_read     = pread(ssd_fd, buffer, to_read, file_offset);
    if (bytes_read <= 0) {
      LOG(ERR, "Failed to read from SSD at offset %ld", file_offset);
      fclose(local_file);
      close(ssd_fd);
      return -1;
    }

    fwrite(buffer, 1, bytes_read, local_file);
    file_size -= bytes_read;
    file_offset += bytes_read;
  }

  fclose(local_file);
  close(ssd_fd);

  LOG(INFO, "File '%s' downloaded successfully", basename);
  return 0;
}

int delete_file(config_context_t cfg_ctx, const char *filename) {
  LOG(INFO, "Deleting file: %s", filename);
  // open SSD
  int ssd_fd = open(cfg_ctx.drive_full_path, O_RDWR);
  if (ssd_fd == -1) {
    LOG(ERR, "Error opening SSD");
    return -1;
  }

  // read the metadata table
  std::vector<storage_metadata_t> md_table = md_table_read(ssd_fd);

  // search for filename in metadata table
  LOG(INFO, "Searching for file: %s in metadata table", filename);
  // set index to something invalid
  size_t file_index = md_table.size();
  for (size_t i = 0; i < md_table.size(); ++i) {
    if (strcmp(md_table[i].filename, filename) == 0) {
      file_index = i;
      break;
    }
  }

  if (file_index == md_table.size()) {
    LOG(WARN, "File %s not found in metadata table", filename);
    close(ssd_fd);
    return -1;
  }

  const storage_metadata_t &file_entry = md_table[file_index];
  LOG(INFO,
      "Found file %s with offset 0x%x and size %u bytes",
      file_entry.filename,
      file_entry.start_offset,
      file_entry.size);

  // delete file content by writing a 0'd out buffer
  std::vector<unsigned char> reset_buffer(file_entry.size, 0);
  if (lseek(ssd_fd, file_entry.start_offset, SEEK_SET) == -1) {
    LOG(ERR, "Failed to seek to file offset 0x%x", file_entry.start_offset);
    close(ssd_fd);
    return -1;
  }

  LOG(INFO,
      "Deleting file by writing zeroes at offset 0x%x",
      file_entry.start_offset);
  ssize_t written = write(ssd_fd, reset_buffer.data(), file_entry.size);
  if (written != static_cast<ssize_t>(file_entry.size)) {
    LOG(ERR,
        "Failed to write zeroes to file at offset 0x%x",
        file_entry.start_offset);
    close(ssd_fd);
    return -1;
  }
  LOG(INFO,
      "Successfully erased %u bytes for file %s",
      file_entry.size,
      file_entry.filename);

  // remove the metadata entry
  LOG(INFO, "Deleting metadata entry for file {%s}", file_entry.filename);
  md_table.erase(md_table.begin() + file_index);

  // rewrite the updated metadata table back to the SSD
  LOG(INFO, "Rewriting metadata table");
  for (size_t i = 0; i < md_table.size(); ++i) {
    if (!md_table_write(ssd_fd, md_table[i], i)) {
      LOG(ERR, "Failed to write metadata table entry %zu", i);
      close(ssd_fd);
      return -1;
    }
  }

  // zero out the unused metadata entry space
  storage_metadata_t empty_entry = {};
  size_t empty_index             = md_table.size();
  LOG(INFO, "Zeroing out unused metadata entry space");
  if (!md_table_write(ssd_fd, empty_entry, empty_index)) {
    LOG(ERR,
        "Failed to clear the unused metadata entry at index %zu",
        empty_index);
    close(ssd_fd);
    return -1;
  }

  LOG(INFO,
      "Successfully deleted file %s and updated metadata table",
      filename);
  // close ssd file desc
  close(ssd_fd);
  return 0;
}

int list_files(config_context_t cfg_ctx) {
  LOG(INFO, "Listing all files on the drive");

  // open SSD
  int ssd_fd = open(cfg_ctx.drive_full_path, O_RDWR);
  if (ssd_fd == -1) {
    LOG(ERR, "Error opening SSD");
    return 1;
  }

  // read the metadata table
  std::vector<storage_metadata_t> md_table = md_table_read(ssd_fd);

  LOG(INFO, "Number of files : %d", md_table.size());
  md_table_print(md_table);

  return 0;
}

// SSD I/O functions
/*****************************************************************************/
int ssd_read(unsigned char *buffer, size_t size, off_t offset) {
  int fd = open(DEVICE_PATH, O_RDONLY);
  if (fd == -1) {
    LOG(ERR, "Error opening device {%s}", strerror(errno));
    return 1;
  }

  if (lseek(fd, offset, SEEK_SET) == -1) {
    LOG(ERR,
        "Error seeking to offset {%d} errno {%d}",
        offset,
        strerror(errno));
    close(fd);
    return 1;
  }

  ssize_t read_bytes = read(fd, buffer, size);
  if (read_bytes == -1) {
    LOG(ERR, "Error opening device {%s}", strerror(errno));
    close(fd);
    return 1;
  }

  LOG(INFO,
      "Read {%d} bytes from {%s} at offset {%d}",
      read_bytes,
      DEVICE_PATH,
      offset);
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
    LOG(ERR, "Error opening device {%s}", strerror(errno));
    return 1;
  }

  if (lseek(fd, offset, SEEK_SET) == -1) {
    LOG(ERR,
        "Error seeking to offset {%d} errno {%d}",
        offset,
        strerror(errno));
    close(fd);
    return 1;
  }

  ssize_t written = write(fd, buffer, size);
  if (written == -1) {
    LOG(ERR, "Error writing to device: %d", strerror(errno));
    close(fd);
    return 1;
  }

  LOG(INFO,
      "Wrote {%d} bytes from {%s} at offset {%d}",
      written,
      DEVICE_PATH,
      offset);
  close(fd);
  return 0;
}

// SSD "self" tests
/*****************************************************************************/
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

int ssd_reset(off_t offset, size_t size) {
  // Use std::vector to dynamically allocate a buffer with the desired size
  std::vector<unsigned char> reset_buffer(size,
                                          0); // Initialize buffer with zeroes

  // Open the device
  int fd = open(DEVICE_PATH, O_RDWR);
  if (fd == -1) {
    LOG(ERR, "Error opening device");
    return 1;
  }

  // Seek to the specified offset
  if (lseek(fd, offset, SEEK_SET) == -1) {
    LOG(ERR, "Error seeking to offset %ld in device", offset);
    close(fd);
    return 1;
  }

  // Write the zeroed buffer to reset the section
  ssize_t written = write(fd,
                          reset_buffer.data(),
                          size); // Use .data() to get a pointer to the buffer
  if (written == -1) {
    LOG(ERR, "Error writing to device at offset %ld", offset);
    close(fd);
    return 1;
  }

  LOG(INFO, "Reset %zu bytes at offset %ld", size, offset);

  // Optionally, read back to verify reset (not required for reset
  // functionality)
  std::vector<unsigned char> verify_buffer(size);
  if (lseek(fd, offset, SEEK_SET) == -1) {
    LOG(ERR, "Error seeking to offset %ld for verification", offset);
    close(fd);
    return 1;
  }

  ssize_t read_bytes =
    read(fd, verify_buffer.data(), size); // Use .data() here as well
  if (read_bytes == -1) {
    LOG(ERR, "Error reading from device at offset %ld", offset);
    close(fd);
    return 1;
  }

  if (memcmp(reset_buffer.data(), verify_buffer.data(), size) == 0) {
    LOG(INFO, "Reset verification successful. Region reset to zeroes.");
  } else {
    LOG(INFO, "Reset verification failed. Region not reset as expected.");
  }

  // Close the file descriptor
  close(fd);
  return 0;
}
