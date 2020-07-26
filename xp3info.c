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

void gowrong(char *reason) {
  printf("%s\n", reason);
  exit(EXIT_FAILURE);
}

int getxp3info(char *filepath) {
  FILE *xp3file = NULL;
  struct KRKR2_XP3Header XP3Header;
  struct KRKR2_XP3_DATA_HEADER XP3DataHeader;
  struct KRKR2_XP3_INDEX XP3Index;
  const int32_t magic = 0x50580d33;  // XP3 magic "XP3"

  xp3file = fopen(filepath, "rb");  // open XP3 file

  if (NULL == xp3file) {
    gowrong("Cannot read file!");
  }  // exit when file_fail

  fseek(xp3file, 0, SEEK_SET);
  fread(&XP3Header.Magic, sizeof(XP3Header.Magic), 1, xp3file);
  if (i32conv(XP3Header.Magic) != magic) {
    gowrong("not a valid XP3 archive!");
  }
  printf("Found a XP3 archive\n");
  XP3DataHeader.OriginalSize = XP3Header.offset;
  {
    int64_t offset;

    offset = XP3DataHeader.OriginalSize + SEEK_SET;
    fseek(xp3file, offset, SEEK_SET);
    fread(&XP3DataHeader, sizeof(XP3DataHeader), 1, xp3file);
    printf("\n%x\t%x\n", i64highpart(XP3DataHeader.ArchiveSize),
           i64lowpart(XP3DataHeader.ArchiveSize));
    do {
      if ((i64highpart(XP3DataHeader.ArchiveSize) != 0) ||
          (i64lowpart(XP3DataHeader.ArchiveSize) == 0)) {
        printf("a!\n%x\t%x\n", (i64highpart(XP3DataHeader.ArchiveSize) != 0),
               (i64lowpart(XP3DataHeader.ArchiveSize) == 0));
        continue;
      }
    } while (XP3DataHeader.bZlib & 0x80);

    printf("Orig size:%lld\nComped size:%lld\nComp rate:%d%%\n",
           XP3DataHeader.OriginalSize, XP3DataHeader.ArchiveSize,
           (int)(XP3DataHeader.ArchiveSize / XP3DataHeader.OriginalSize));
  }
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
