#include <stdint.h>
#include <stdio.h>

#define FIRST 1

#define XP3_HEADER_MAGIC 0x50580d33 //XP3 magic "XP3"

#define TVP_XP3_INDEX_ENCODE_METHOD_MASK 0x07
#define TVP_XP3_INDEX_ENCODE_RAW 0
#define TVP_XP3_INDEX_ENCODE_ZLIB 1

#define TVP_XP3_INDEX_CONTINUE 0x80

#define TVP_XP3_FILE_PROTECTED (1 << 31)

#define TVP_XP3_SEGM_ENCODE_METHOD_MASK 0x07
#define TVP_XP3_SEGM_ENCODE_RAW 0
#define TVP_XP3_SEGM_ENCODE_ZLIB 1

#define true 1
#define false 0

typedef unsigned char byte_t;

typedef unsigned char bool;

struct KRKR2_XP3Header
{
  int32_t Magic;
  byte_t m2[0x7]; //placeholder,but necessary.
  int64_t offset;
};

struct KRKR2_XP3_DATA_HEADER
{
  byte_t bZlib;
  int64_t ArchiveSize;
  int64_t OriginalSize;
};

struct KRKR2_XP3_INDEX
{
  struct KRKR2_XP3_DATA_HEADER DataHeader;
  byte_t Index[1];
};

struct portato_xp3info_info
{
  int filetype;//"0" for bare XP3, "1" for XP3 with Win32 PE file, and "-1" for invalid file.
};

int32_t i32conv(int32_t i32)
{
  int32_t i32c = ((i32 & 0xffff0000) >> 16) + ((i32 & 0x0000ffff) << 16);
  return i32c;
}

int64_t i64conv(int64_t i64)
{
  int64_t i64c =
      (((i64 & 0xffff000000000000) >> 16) + ((i64 & 0x0000ffffffff0000) >> 16) +
       ((i64 & 0x000000000000ffff) << 48));
  return i64c;
}

int32_t i64lowpart(int64_t i64)
{
  int32_t i32c = (i64 & 0x00000000ffffffff);
  return i32c;
}

int32_t i64highpart(int64_t i64)
{
  int32_t i32c = (i64 & 0xffffffff00000000);
  return i32c;
}

void gowrong(char *reason)
{ //simplify the error screen
  printf("%s\n", reason);
  exit(EXIT_FAILURE);
}

int isfile(char path[])
{
  struct stat buf;
  int result;
  result = stat(&path[0], &buf);
  if (S_IFREG & buf.st_mode)
  {
    return 1; // file
  }
  return EXIT_SUCCESS;
}