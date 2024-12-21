#include <gtest/gtest.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <chrono>
#include <iostream>

#include "utils.hpp"
#include "storage.hpp"

class UploadFileTest : public ::testing::Test {
protected:
  int ssd_fd;        
  const char *test_filename = "../test_files/wavs/CantinaBand3.wav";
  config_context_t config_ctx = {};

  void SetUp() override {
    // setup fake ssd file descriptor
    char temp_ssd_path[] = "/tmp/fake_ssd_XXXXXX";
    ssd_fd = mkstemp(temp_ssd_path);
    ASSERT_NE(ssd_fd, -1) << "Failed to create temporary SSD file";
    
    config_ctx.drive_full_path = strdup(temp_ssd_path);
    
    test_filename = "../test_files/wavs/CantinaBand3.wav";
  }

  void TearDown() override {
    // Close the SSD device
    if (ssd_fd != -1) {
      close(ssd_fd);
    }
  }
};

TEST_F(UploadFileTest, UploadValidFile) {
  // Upload the file
  int result = upload_file(config_ctx, test_filename);
  EXPECT_EQ(result, 0) << "File upload failed";

  // Check metadata table
  std::vector<storage_metadata_t> metadata_table = metadata_table_read(ssd_fd);
  metadata_table_print(metadata_table);
  ASSERT_FALSE(metadata_table.empty()) << "Metadata table is empty";

  bool file_found = false;
  // search for the filename in the metadata table
  auto it = std::find_if(metadata_table.begin(),
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
