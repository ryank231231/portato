#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>

#define FIRST 1
#define XP3_TAG1 0x656C6946
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
  int32_t *bf32=malloc(sizeof(int32_t));
  int64_t bf64;
  printf("filename:%s\n", filepath);
  xp3file = fopen(filepath, "rb");
  if (NULL == xp3file) {
    printf("cannot open file!\n");
    return 1;
  }
  
  fseek(xp3file,0,SEEK_SET);
  printf("seek it\n");
  printf("%d\n",sizeof(int32_t));
  fread(bf32,sizeof(int32_t),1,xp3file);
  printf("read it\n");
  printf("0x%x\n",*bf32);
  if(*bf32==XP3_TAG1){
      return 0;
  }else{
      printf("not a valid xp3 file!\n");
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
