#include <stdint.h>
#include <stdio.h>

#define FIRST 1

typedef unsigned char byte_t;

struct KRKR_XP3Header {
  int32_t Magic;
  byte_t m2[0x7];
  int64_t offset;
};

struct sXP3info {
  byte_t zlib;
  int64_t psize;
  int64_t rsize;
  byte_t *fileinfo;
};

int32_t i32conv(int32_t i32) {
  int32_t i32c = ((i32 & 0xffff0000) >> 16) + ((i32 & 0x0000ffff) << 16);
  return i32c;
}

int64_t i64conv(int64_t i64) {
  int64_t i64c =
      (((i64 & 0xffff000000000000) >> 16) + ((i64 & 0x0000ffffffff0000) >> 16) +
       ((i64 & 0x000000000000ffff) << 48));
  return i64c;
}