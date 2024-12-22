#include <gtest/gtest.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <chrono>
#include <iostream>
#include <fstream>

#include "utils.hpp"
#include "storage.hpp"

class UploadFileTest : public ::testing::Test {
protected:
  int ssd_fd;
  const char *test_filename   = "../test_files/wavs/CantinaBand3.wav";
  config_context_t config_ctx = {};

  void SetUp() override {
    // setup fake ssd file descriptor
    char temp_ssd_path[] = "/tmp/fake_ssd_XXXXXX";
    ssd_fd               = mkstemp(temp_ssd_path);
    ASSERT_NE(ssd_fd, -1) << "Failed to create temporary SSD file";

    config_ctx.drive_full_path = strdup(temp_ssd_path);

    test_filename = "../test_files/wavs/CantinaBand3.wav";
  }

  void TearDown() override {
    if (ssd_fd != -1) {
      close(ssd_fd);
    }
  }
};

TEST_F(UploadFileTest, UploadValidFile) {
  int result = upload_file(config_ctx, test_filename);
  EXPECT_EQ(result, 0) << "File upload failed";

  std::vector<storage_metadata_t> metadata_table = metadata_table_read(ssd_fd);
  metadata_table_print(metadata_table);
  ASSERT_FALSE(metadata_table.empty()) << "Metadata table is empty";

  bool file_found = false;
  auto it =
    std::find_if(metadata_table.begin(),
                 metadata_table.end(),
                 [this](const storage_metadata_t &entry) {
                   return strcmp(entry.filename, this->test_filename) == 0;
                 });

  if (it == metadata_table.end()) {
    LOG(ERR, "File '%s' not found on SSD.", test_filename);
    file_found = false;
  } else {
    file_found = true;
  }

  EXPECT_TRUE(file_found) << "Uploaded file not found in metadata table";
}

TEST_F(UploadFileTest, DownloadValidFile) {
  // upload the file first
  ASSERT_EQ(upload_file(config_ctx, test_filename), 0) << "File upload failed";

  // download the file
  int result = download_file(config_ctx, test_filename);
  EXPECT_EQ(result, 0) << "File download failed";

  // Verify the downloaded file
  const char *basename = strrchr(test_filename, '/');
  basename             = (basename) ? basename + 1 : test_filename;

  // open original and downloaded files
  std::ifstream original_file(test_filename, std::ios::binary | std::ios::ate);
  std::ifstream downloaded_file(basename, std::ios::binary | std::ios::ate);

  ASSERT_TRUE(original_file.is_open()) << "Failed to open original test file";
  ASSERT_TRUE(downloaded_file.is_open()) << "Failed to open downloaded file";

  // compare file sizes
  std::streamsize original_size   = original_file.tellg();
  std::streamsize downloaded_size = downloaded_file.tellg();
  EXPECT_EQ(original_size, downloaded_size) << "File sizes do not match";

  // compare file contents
  original_file.seekg(0);
  downloaded_file.seekg(0);
  std::vector<char> original_buffer(original_size);
  std::vector<char> downloaded_buffer(downloaded_size);

  original_file.read(original_buffer.data(), original_size);
  downloaded_file.read(downloaded_buffer.data(), downloaded_size);

  EXPECT_EQ(original_buffer, downloaded_buffer) << "File contents do not match";

  std::remove(basename);
}
