#include <gtest/gtest.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <chrono>
#include <iostream>

#include "storage.hpp"

class UploadFileTest : public ::testing::Test {
protected:
  int ssd_fd;                // File descriptor for SSD
  const char* test_filename; // Path to the test file
  config_context_t config_ctx = {};

  void SetUp() override {
    // Open the SSD device
    ssd_fd = open(DEVICE_PATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    ASSERT_NE(ssd_fd, -1) << "Failed to open SSD device";

    // Path to the test file
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
  for (const auto& entry : metadata_table) {
    std::cout << entry.filename <<std::endl;
    
    if (strcmp(entry.filename, "../test_files/wavs/CantinaBand60.wav") == 0) {
      file_found = true;
      EXPECT_EQ(entry.size, 6180) << "File size mismatch";
      EXPECT_EQ(entry.start_offset, 4096) << "Start offset mismatch";
      break;
    }
  }

  EXPECT_TRUE(file_found) << "Uploaded file not found in metadata table";
}
