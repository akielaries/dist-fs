#include <gtest/gtest.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#include "storage.hpp"

// Mock data for metadata table
const size_t MOCK_METADATA_ENTRIES                      = 3;
storage_metadata_t mock_md_table[MOCK_METADATA_ENTRIES] = {
  {"file1.txt", 1024, 512, false},
  {"file2.txt", 2048, 1024, false},
  {"dir1", 3072, 0, true}};

class StorageDriverTest : public ::testing::Test {
protected:
  int mock_fd;
  char mock_ssd[METADATA_TABLE_SZ];

  void SetUp() override {
    memset(mock_ssd, 0, METADATA_TABLE_SZ);
    memcpy(mock_ssd, mock_md_table, sizeof(mock_md_table));

    mock_fd = open("/tmp/mock_ssd", O_RDWR | O_CREAT | O_TRUNC, 0666);
    ASSERT_NE(mock_fd, -1) << "Failed to create mock SSD file";

    ssize_t bytes_written = write(mock_fd, mock_ssd, METADATA_TABLE_SZ);
    ASSERT_EQ(bytes_written, METADATA_TABLE_SZ)
      << "Failed to write mock SSD data";

    lseek(mock_fd, 0, SEEK_SET);
  }

  void TearDown() override {
    close(mock_fd);
    unlink("/tmp/mock_ssd");
  }
};

// reading metadata table with valid entries
TEST_F(StorageDriverTest, ReadValidMetadataTable) {
  std::vector<storage_metadata_t> metadata = md_table_read(mock_fd);

  ASSERT_EQ(metadata.size(), MOCK_METADATA_ENTRIES);

  for (size_t i = 0; i < MOCK_METADATA_ENTRIES; ++i) {
    EXPECT_STREQ(metadata[i].filename, mock_md_table[i].filename);
    EXPECT_EQ(metadata[i].start_offset, mock_md_table[i].start_offset);
    EXPECT_EQ(metadata[i].size, mock_md_table[i].size);
    EXPECT_EQ(metadata[i].is_directory, mock_md_table[i].is_directory);
  }
}

// reading empty metadata table
TEST_F(StorageDriverTest, ReadEmptyMetadataTable) {
  memset(mock_ssd, 0, METADATA_TABLE_SZ);
  lseek(mock_fd, 0, SEEK_SET);
  write(mock_fd, mock_ssd, METADATA_TABLE_SZ);
  lseek(mock_fd, 0, SEEK_SET);

  std::vector<storage_metadata_t> metadata = md_table_read(mock_fd);

  EXPECT_TRUE(metadata.empty());
}

// handle seek failure
TEST_F(StorageDriverTest, SeekFailure) {
  close(mock_fd);

  std::vector<storage_metadata_t> metadata = md_table_read(mock_fd);

  EXPECT_TRUE(metadata.empty());
}

// handle partial read
TEST_F(StorageDriverTest, PartialRead) {
  ssize_t partial_size =
    sizeof(storage_metadata_t) * (MOCK_METADATA_ENTRIES - 1);
  lseek(mock_fd, 0, SEEK_SET);
  write(mock_fd, mock_md_table, partial_size);
  lseek(mock_fd, 0, SEEK_SET);

  std::vector<storage_metadata_t> metadata = md_table_read(mock_fd);

  ASSERT_EQ(metadata.size(), MOCK_METADATA_ENTRIES);
  for (size_t i = 0; i < MOCK_METADATA_ENTRIES - 1; ++i) {
    EXPECT_STREQ(metadata[i].filename, mock_md_table[i].filename);
    EXPECT_EQ(metadata[i].start_offset, mock_md_table[i].start_offset);
    EXPECT_EQ(metadata[i].size, mock_md_table[i].size);
    EXPECT_EQ(metadata[i].is_directory, mock_md_table[i].is_directory);
  }
}
