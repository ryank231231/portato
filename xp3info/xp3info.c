#include "xp3info.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../includes/zlib/zlib.h"
#include "../portato.h"

int help(void) {
  printf("Usage: xp3info <filename>\n\n");
  printf("Written by ryank231231 <https://ryank231231.top>\n");
  printf("This program is covered by terms of the GPL Version 3.\n");
  return EXIT_SUCCESS;
}

uint64_t getheaderinXP3(FILE *filest) {
  fseek(filest, 0, SEEK_SET);
  return 0;  // no need to find offset in pure XP3
}

uint64_t getheaderinPE(FILE *filest, uint8_t *mark, uint8_t *XP3Mark) {
  bool found = false;
  uint64_t offset = 16;

  fseek(filest, 16, SEEK_SET);

  const unsigned int one_read_size = 256 * 1024;  // read 256kb a time
  unsigned int read;
  uint8_t buffer[one_read_size];

  while (1 == fread(buffer, one_read_size, 1, filest)) {
    read = one_read_size;
    unsigned int p = 0;
    while (p < read) {
      if (memcmp(XP3Mark, buffer + p, sizeof(XP3Mark) - 1) == 0) {
        offset += p;
        found = true;
        break;
      }

      p += 16;
    }

    if (found == true) break;

    offset += one_read_size;

    if (found == false) {
      fseek(filest, one_read_size, offset);  // read next block
      continue;
    }
  }

  if (found == false) {
    gowrong(FILE_NOT_VAILD);
  }

  return offset;
}

void printoffset(FILE *file) {
  printf("offset : %ld(%lX)\n", ftell(file), ftell(file));
}

int getxp3info(char *filepath) {
  FILE *filest = NULL;
  uint8_t mark[11];  // mark read from file stream
  struct KRKR2_XP3_INDEX XP3Index;
  struct portato_xp3info_info infotable;
  const uint8_t XP3Mark1[] = {
      0x58 /*'X'*/, 0x50 /*'P'*/,  0x33 /*'3'*/, 0x0d /*'\r'*/, 0x0a /*'\n'*/,
      0x20 /*' '*/, 0x0a /*'\n'*/, 0x1a /*EOF*/, 0xff /* sentinel */};
  const uint8_t XP3Mark2[] = {0x8b, 0x67, 0x01, 0xff /* sentinel */};
  uint8_t XP3Mark[11];  // for comparing
  const uint8_t MZMark[] = {
      0x4d /*'M'*/, 0x5a /*'Z'*/
  };
  const uint8_t cn_File[] = {0x46 /*'F'*/, 0x69 /*'i'*/, 0x6c /*'l'*/,
                             0x65 /*'e'*/};
  const uint8_t cn_info[] = {0x69 /*'i'*/, 0x6e /*'n'*/, 0x66 /*'f'*/,
                             0x6f /*'o'*/};
  const uint8_t cn_segm[] = {0x73 /*'s'*/, 0x65 /*'e'*/, 0x67 /*'g'*/,
                             0x6d /*'m'*/};
  const uint8_t cn_adlr[] = {0x61 /*'a'*/, 0x64 /*'d'*/, 0x6c /*'l'*/,
                             0x72 /*'r'*/};
  bool DoInit = true;
  int segmentcount;

  if (DoInit) {
    // the XP3 header above is splitted into two part; to avoid
    // mis-finding of the header in the program's initialized data area.
    DoInit = false;
    memcpy(XP3Mark, XP3Mark1, 8);
    memcpy(XP3Mark + 8, XP3Mark2, 3);
    // here joins it.
    mark[0] = 0;  // sentinel
  }

  filest = fopen(filepath, "rb");  // open XP3 file

  if (NULL == filest) {
    gowrong(CANNOT_READ_FILE);
  }  // exit when readfile_fail

  fread(&mark, sizeof(mark), 1, filest);  // read magic

  if (memcmp(XP3Mark, mark, sizeof(mark)) == 0) {
    infotable.offset = 0;
    infotable.filetype = 0;  // set for bare XP3
  } else {
    if ((memcmp(MZMark, mark, sizeof(MZMark))) == 0) {
      infotable.offset = getheaderinPE(filest, mark, XP3Mark);
      infotable.filetype = 1;  // by the way~
    } else {
      gowrong(FILE_NOT_VAILD);
    }
  }

  printf("Trying to read XP3 virtual file system information from : %s\n",
         filepath);

  printf("File type: ");
  switch (infotable.filetype) {
    case 0:
      printf("bare XP3 archive\n");
      break;

    case 1:
      printf("XP3 archive bundled with Win32 PE (Portable Executable)\n");
      printf("bundled XP3 offset: %" PRIu64 " (%" PRIX64 ")\n",
             infotable.offset, infotable.offset);
    default:
      break;
  }

  // next part

  segmentcount = 0;
  fseek(filest, infotable.offset + 11, SEEK_SET);
  // read next table
  XP3Index.index_ofs = ReadI64LE(filest);
  printf("XP3 Index offset: %" PRIu64 " (%" PRIX64 ")\n", XP3Index.index_ofs,
         XP3Index.index_ofs);
  fseek(filest, XP3Index.index_ofs, SEEK_SET);  // seek to xp3 #1 index
#ifdef DEBUG
  printoffset(filest);
#endif
  fread(&XP3Index.index_flag, sizeof(XP3Index.index_flag), 1,
        filest);  // it will move pointers automatically
#ifdef DEBUG
  printoffset(filest);
#endif
  XP3Index.DataHeader.ArchiveSize = ReadI64LE(filest);

  if ((XP3Index.index_flag & TVP_XP3_INDEX_ENCODE_METHOD_MASK) ==
      TVP_XP3_INDEX_ENCODE_ZLIB) {
#ifdef DEBUG
    printoffset(filest);
    printf("RAW XP3Index.index_flag: %x\n", XP3Index.index_flag);
#endif
    XP3Index.DataHeader.OriginalSize = ReadI64LE(filest);
    printf("Index is compressed by Zlib\n");
    printf("Index size: %" PRIu64 " (%" PRIX64 ")\t",
           XP3Index.DataHeader.ArchiveSize, XP3Index.DataHeader.ArchiveSize);
    printf("Index size(origin): %" PRIu64 " (%" PRIX64 ")\n",
           XP3Index.DataHeader.OriginalSize, XP3Index.DataHeader.OriginalSize);

    uint8_t indexdata[XP3Index.DataHeader.OriginalSize];
    uint8_t *indexdata_compressed =
        malloc(sizeof(uint8_t) * XP3Index.DataHeader.ArchiveSize);

    fread(indexdata_compressed, XP3Index.DataHeader.ArchiveSize, 1, filest);

    unsigned long destlen=(unsigned long)XP3Index.DataHeader.OriginalSize;

    uncompress(indexdata, &destlen,
               indexdata_compressed, XP3Index.DataHeader.ArchiveSize);

    free(indexdata_compressed);
  } else if ((XP3Index.index_flag & TVP_XP3_INDEX_ENCODE_METHOD_MASK) ==
             TVP_XP3_INDEX_ENCODE_RAW) {
    printf("Index is raw\n");
    printf("Index size: %" PRIu64 " (%" PRIX64 ")\n",
           XP3Index.DataHeader.ArchiveSize, XP3Index.DataHeader.ArchiveSize);
  }

  if ((XP3Index.index_flag & TVP_XP3_INDEX_ENCODE_METHOD_MASK) ==
      TVP_XP3_INDEX_ENCODE_RAW) {
    printf("Index is raw\n");
    printf("Index size: %" PRIu64 " (%" PRIX64 ")\n",
           XP3Index.DataHeader.ArchiveSize, XP3Index.DataHeader.ArchiveSize);
  }

  // end
  fclose(filest);
  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
#ifdef DEBUG
  printf("DEBUG MODE\txp3info build on %s %s\n", __DATE__, __TIME__);
#endif
  if (argc == 1) {
    help();
  } else {
    if (isfile(argv[FIRST])) {
      getxp3info(argv[FIRST]);
    }
  }
}
