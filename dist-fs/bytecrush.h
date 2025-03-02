#pragma once

#include <stdint.h>


#define MAX_TREE_NODES 512
#define BYTE_RANGE     256


typedef struct {
  uint8_t byte;
  char code[32];
} huffman_code_t;

typedef struct {
  uint32_t original_size;
  uint32_t compressed_size;
} wav_header_t;

huffman_code_t huffman_table[BYTE_RANGE];


int bytecrush_compress(const char *file_path);

int bytecrush_decompress(const char *file_path);
