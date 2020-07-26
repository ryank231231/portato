#include <stdint.h>
#include <stdio.h>

#define FIRST 1

typedef unsigned char byte_t;

struct KRKR2_XP3Header {
  int32_t Magic;
  byte_t m2[0x7];//placeholder,but necessary.
  int64_t offset;
};

struct KRKR2_XP3_DATA_HEADER {
  byte_t bZlib;
  int64_t ArchiveSize;
  int64_t OriginalSize;
};

struct KRKR2_XP3_INDEX {
  struct KRKR2_XP3_DATA_HEADER DataHeader;
  byte_t Index[1];
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

int32_t i64lowpart(int64_t i64){
  int32_t i32c =(i64&0x00000000ffffffff);
  return i32c;
}

int32_t i64highpart(int64_t i64){
  int32_t i32c =(i64&0xffffffff00000000);
  return i32c;
}