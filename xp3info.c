#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>

#define FIRST 1

int i32conv(int32_t i32) {
  int32_t i32c = ((i32 & 0xffff0000) >> 16) + ((i32 & 0x0000ffff) << 16);
  return i32c;
}
int isfile(char path[]) {
  struct stat buf;

  int result;
  result = stat(&path[0], &buf);
  if (S_IFREG & buf.st_mode) {
    return 1; // file
  }
  return 0;
}

int help(void) {
  printf("Usage: xp3info <filename>\n");
  return 0;
}

int getxp3info(char *filepath) {
  FILE *xp3file = NULL;
  int32_t *bf32 = malloc(sizeof(int32_t));
  int64_t *bf64 = malloc(sizeof(int64_t));

  const int32_t magic = 0x50580d33; // XP3 magic "XP3"

  xp3file = fopen(filepath, "rb");
  if (NULL == xp3file) {
    printf("cannot open file!\n");
    return 1;
  }

  fseek(xp3file, 0, SEEK_SET);
  fread(bf32, sizeof(int32_t), 1, xp3file);
  printf("0x%x,0x%x\n", i32conv(*bf32), magic);
  if (~(*bf32 == magic)) {
    printf("Not a valid XP3 file!\n");
  }
  printf("XP3\n");
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
