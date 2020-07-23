#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>

#define FIRST 1

typedef unsigned char byte_t;

struct sXP3info{
    byte_t zlib;
    int64_t psize;
    int64_t rsize;
    byte_t *fileinfo;
};

int32_t i32conv(int32_t i32) {
  int32_t i32c = ((i32 & 0xffff0000) >> 16) + ((i32 & 0x0000ffff) << 16);
  return i32c;
}

int64_t i64conv(int64_t i64){
    int64_t i64c = (((i64&0xffff000000000000)>>16)+((i64&0x0000ffffffff0000)>>16)+((i64&0x000000000000ffff)<<48));
    return i64c;
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
  struct sXP3info xp3h_ifo;
  
  const int32_t magic = 0x50580d33; // XP3 magic "XP3"

  xp3file = fopen(filepath, "rb");
  if (NULL == xp3file) {
    printf("cannot open file!\n");
    return 1;
  }

  fseek(xp3file, 0, SEEK_SET);
  fread(bf32, sizeof(int32_t), 1, xp3file);
  if ((i32conv(*bf32))==magic){
      printf("a valid xp3 file\n");
      fseek(xp3file,11,SEEK_SET);
      fread(bf64,sizeof(int64_t),1,xp3file);
      printf("raw xp3 offset:%lX,%lX\n",*bf64,i64conv(*bf64));
      fseek(xp3file,*bf64,SEEK_SET);
      fread(&xp3h_ifo,sizeof(xp3h_ifo),1,xp3file);
      printf("%d\t",xp3h_ifo.zlib);
      printf("%ld\\%ld\n",xp3h_ifo.psize,xp3h_ifo.rsize);
  }else{
      printf("not a valid xp3 file\n");
      return 1;
  }
  fclose(xp3file);
  free(bf32);
  free(bf64);
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
