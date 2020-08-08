#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define FIRST 1

#define TVP_XP3_INDEX_ENCODE_METHOD_MASK 0x07
#define TVP_XP3_INDEX_ENCODE_RAW 0
#define TVP_XP3_INDEX_ENCODE_ZLIB 1

#define TVP_XP3_INDEX_CONTINUE 0x80

#define TVP_XP3_FILE_PROTECTED (1 << 31)

#define TVP_XP3_SEGM_ENCODE_METHOD_MASK 0x07
#define TVP_XP3_SEGM_ENCODE_RAW 0
#define TVP_XP3_SEGM_ENCODE_ZLIB 1

#ifdef WORDS_BIGENDIAN
#define HOST_IS_BIG_ENDIAN 1
#define HOST_IS_LITTLE_ENDIAN 0
#else
#define HOST_IS_BIG_ENDIAN 0
#define HOST_IS_LITTLE_ENDIAN 1
#endif

#define true 1
#define false 0

typedef unsigned char byte_t;

typedef unsigned char bool;

struct KRKR2_XP3_DATA_HEADER {
  byte_t bZlib;
  uint64_t ArchiveSize;
  uint64_t OriginalSize;
};

struct KRKR2_XP3_INDEX {
  uint64_t index_ofs;
  uint8_t index_flag;
  unsigned int index_size;
  struct KRKR2_XP3_DATA_HEADER DataHeader;
};

struct portato_xp3info_info {
  int filetype;  //"0" for bare XP3, "1" for XP3 with Win32 PE file, and "-1"
                 // for invalid file.
  uint64_t offset;
};

int32_t i32conv(int32_t i32) {
  int32_t i32c = ((i32 & 0xffff0000) >> 16) + ((i32 & 0x0000ffff) << 16);
  return i32c;
}

uint64_t i64conv(uint64_t i64) {
  uint64_t i64c =
      ((i64 & 0x00ff000000000000) >> 40) + ((i64 & 0x0000ff0000000000) >> 24) +
      ((i64 & 0x000000ff00000000) >> 8) + ((i64 & 0x00000000ff000000) << 8) +
      ((i64 & 0x0000000000ff0000) << 24) + ((i64 & 0x000000000000ff00) << 40) +
      ((i64 & 0xffffffffffffffff) << 56) + ((i64 & 0xff00000000000000) >> 56);
  return i64c;
}

int32_t i64lowpart(int64_t i64) {
  int32_t i32c = (i64 & 0x00000000ffffffff);
  return i32c;
}

int32_t i64highpart(int64_t i64) {
  int32_t i32c = (i64 & 0xffffffff00000000);
  return i32c;
}

uint64_t ReadI64LE(FILE *file) {
#if HOST_IS_BIG_ENDIAN
  uint8_t buffer[8];
  fread(&buffer, 8, 1, file);
  uint64_t ret = 0;
  for (int i = 0; i < 8; i++) ret += (uint64_t)buffer[i] << (i * 8);
  return ret;
#else
  uint64_t temp;
  fread(&temp, 8, 1, file);
  return temp;
#endif
}

uint32_t ReadI32LE(FILE *file) {
#if HOST_IS_BIG_ENDIAN
  uint8_t buffer[4];
  fread(&buffer, 4, 1, file);
  uint32_t ret = 0;
  for (int i = 0; i < 4; i++) ret += (uint32_t)buffer[i] << (i * 8);
  return ret;
#else
  uint32_t temp;
  fread(&temp, 4, 1, file);
  return temp;
#endif
}

uint16_t ReadI16LE(FILE *file) {
#if HOST_IS_BIG_ENDIAN
  uint8_t buffer[2];
  fread(&buffer, 2, 1, file);
  uint16_t ret = 0;
  for (int i = 0; i < 2; i++) ret += (uint16_t)buffer[i] << (i * 8);
  return ret;
#else
  uint16_t temp;
  fread(&temp, 2, 1, file);
  return temp;
#endif
}

void gowrong(char *reason) {  // simplify the error screen
  printf("%s\n", reason);
  exit(EXIT_FAILURE);
}

int isfile(char path[]) {
  struct stat buf;
  int result;
  result = stat(&path[0], &buf);
  if (S_IFREG & buf.st_mode) {
    return 1;  // file
  }
  return EXIT_SUCCESS;
}