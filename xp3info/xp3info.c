#include "xp3info.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>

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

int getxp3info(char *filepath) {
  FILE *filest = NULL;
  uint8_t mark[11];  // mark read from file stream
  struct KRKR2_XP3Header XP3Header;
  struct KRKR2_XP3_DATA_HEADER XP3DataHeader;
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
  uint64_t offset;
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
    offset = 0;
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

  segmentcount = 0;
  fseek(filest, infotable.offset + 11, SEEK_SET);

  fread(&XP3DataHeader, sizeof(XP3DataHeader), 1, filest);

  printf("File type: ");
  switch (infotable.filetype) {
    case 0:
      printf("bare XP3 archive\n");
      break;

    case 1:
      printf("XP3 archive bundled with Win32 PE (Portable Executable)\n");
      printf("bundled XP3 offset: %"PRIu64"\n",infotable.offset);
    default:
      break;
  }



  fclose(filest);
  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    help();
  } else {
    if (isfile(argv[FIRST])) {
      getxp3info(argv[FIRST]);
    }
  }
}
