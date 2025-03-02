#include <gtest/gtest.h>
#include <vector>
#include <cstring>

#include "bytecrush.h"

extern "C" {
int bytecrush_compress(const unsigned char *input,
                       size_t input_size,
                       unsigned char *output,
                       size_t *output_size);
int bytecrush_decompress(const unsigned char *input,
                         size_t input_size,
                         unsigned char *output,
                         size_t *output_size);
}

// Utility function to compare byte arrays
bool compare_bytes(const unsigned char *a,
                   const unsigned char *b,
                   size_t size) {
  return std::memcmp(a, b, size) == 0;
}

TEST(ByteCrushTest, CompressDecompressBasic) {
  unsigned char input[] = "aaabbbcccaaabbbccc"; // Sample repeating pattern
  size_t input_size     = sizeof(input) - 1;    // Exclude null terminator

  unsigned char compressed[256] = {0};
  size_t compressed_size        = sizeof(compressed);

  // Compress the data
  ASSERT_EQ(bytecrush_compress(input, input_size, compressed, &compressed_size),
            0);
  ASSERT_GT(compressed_size, 0); // Ensure compression happened

  unsigned char decompressed[256] = {0};
  size_t decompressed_size        = sizeof(decompressed);

  // Decompress and verify
  ASSERT_EQ(bytecrush_decompress(compressed,
                                 compressed_size,
                                 decompressed,
                                 &decompressed_size),
            0);
  ASSERT_EQ(decompressed_size, input_size);
  ASSERT_TRUE(compare_bytes(input, decompressed, input_size));
}

TEST(ByteCrushTest, CompressEmptyInput) {
  unsigned char input[] = "";
  size_t input_size     = 0;

  unsigned char compressed[256] = {0};
  size_t compressed_size        = sizeof(compressed);

  // Compress an empty input
  ASSERT_EQ(bytecrush_compress(input, input_size, compressed, &compressed_size),
            0);
  ASSERT_EQ(compressed_size, 0);
}

TEST(ByteCrushTest, CompressSingleCharacter) {
  unsigned char input[] = "AAAAAAAAAAAA";
  size_t input_size     = sizeof(input) - 1;

  unsigned char compressed[256] = {0};
  size_t compressed_size        = sizeof(compressed);

  // Compress repeated single character
  ASSERT_EQ(bytecrush_compress(input, input_size, compressed, &compressed_size),
            0);
  ASSERT_GT(compressed_size, 0);

  unsigned char decompressed[256] = {0};
  size_t decompressed_size        = sizeof(decompressed);

  // Decompress and verify
  ASSERT_EQ(bytecrush_decompress(compressed,
                                 compressed_size,
                                 decompressed,
                                 &decompressed_size),
            0);
  ASSERT_EQ(decompressed_size, input_size);
  ASSERT_TRUE(compare_bytes(input, decompressed, input_size));
}

TEST(ByteCrushTest, CompressAlreadyCompressedData) {
  unsigned char input[] = "0123456789abcdef";
  size_t input_size     = sizeof(input) - 1;

  unsigned char compressed[256] = {0};
  size_t compressed_size        = sizeof(compressed);

  // Try compressing data with no repeating patterns
  ASSERT_EQ(bytecrush_compress(input, input_size, compressed, &compressed_size),
            0);
  // May be equal or slightly larger due to overhead
  ASSERT_GE(compressed_size, input_size);

  unsigned char decompressed[256] = {0};
  size_t decompressed_size        = sizeof(decompressed);

  // Decompress and verify
  ASSERT_EQ(bytecrush_decompress(compressed,
                                 compressed_size,
                                 decompressed,
                                 &decompressed_size),
            0);
  ASSERT_EQ(decompressed_size, input_size);
  ASSERT_TRUE(compare_bytes(input, decompressed, input_size));
}
