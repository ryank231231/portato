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
  return EXIT_SUCCESS;
}

int help(void) {
  printf("Usage: xp3info <filename>\n");
  return EXIT_SUCCESS;
}

int getxp3info(char *filepath) {
  FILE *xp3file = NULL;
  struct KRKR_XP3Header XP3Header;
  const int32_t magic = 0x50580d33;  // XP3 magic "XP3"

  xp3file = fopen(filepath, "rb");  // open XP3 file

  if (NULL == xp3file) {
    printf("cannot open file!\n");
    return EXIT_FAILURE;
  }

  fseek(xp3file, 0, SEEK_SET);
  fread(&XP3Header.Magic, sizeof(XP3Header.Magic), 1, xp3file);
  if (i32conv(XP3Header.Magic) != magic) {
    printf("not a valid XP3 archive!\n");
    exit(EXIT_FAILURE);
  }
  fseek(xp3file, 0, SEEK_SET);

  fclose(xp3file);

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
