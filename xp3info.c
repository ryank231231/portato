#include <stdio.h>
#include <sys/stat.h>

#define FIRST 1

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
  printf("filename:%s\n", filepath);
  xp3file = fopen(filepath, "rb");
  if (NULL == xp3file) {
    printf("cannot open file!\n");
    return 1;
  }
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
