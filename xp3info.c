#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "portato.h"

int isfile(char path[]) {
  struct stat buf;

  int result;
  result = stat(&path[0], &buf);
  if (S_IFREG & buf.st_mode) {
    return 1;  // file
  }
  return 0;
}

int help(void) {
  printf("Usage: xp3info <filename>\n");
  return 0;
}

int getxp3info(char *filepath) {
  FILE *xp3file = NULL;
  struct XP3Header XP3H1;

  // const int32_t magic = 0x50580d33;  // XP3 magic "XP3"

  byte_t XP3Mark[0xB] = {0x58 /*'X'*/,  0x50 /*'P'*/,  0x33 /*'3'*/,
                         0x0d /*'\r'*/, 0x0a /*'\n'*/, 0x20 /*' '*/,
                         0x0a /*'\n'*/, 0x1a /*EOF*/,  0x8b,
                         0x67,          0x01};

  xp3file = fopen(filepath, "rb");  // open XP3 file

  if (NULL == xp3file) {
    printf("cannot open file!\n");
    return 1;
  }

  fseek(xp3file, 0, SEEK_SET);
  // fread(bf32, sizeof(int32_t), 1, xp3file);
  fread(&XP3H1, sizeof(XP3H1.Magic), 1, xp3file);
  printf("AS:%s\n", XP3H1.Magic);
  printf("BS:%s\n", (char *)XP3Mark);
  if (0) {
    printf("it a vaild file!\n");
    return 0;
  }
  fseek(xp3file, 0, SEEK_SET);

  fclose(xp3file);

  return 0;
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
