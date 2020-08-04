#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../portato.h"

int help(void)
{
  printf("Usage: xp3info <filename>\n\n");
  printf("Written by ryank231231 <https://ryank231231.top>\n");
  printf("This program is covered by terms of the GPL Version 3.\n");
  return EXIT_SUCCESS;
}

int getxp3info(char *filepath)
{
  FILE *filest = NULL;
  uint8_t mark[11];//mark read from file stream
  struct KRKR2_XP3Header XP3Header;
  struct KRKR2_XP3_DATA_HEADER XP3DataHeader;
  struct KRKR2_XP3_INDEX XP3Index;
  const uint8_t XP3Mark1[] =
		{ 0x58/*'X'*/, 0x50/*'P'*/, 0x33/*'3'*/, 0x0d/*'\r'*/,
		  0x0a/*'\n'*/, 0x20/*' '*/, 0x0a/*'\n'*/, 0x1a/*EOF*/,
		  0xff /* sentinel */ };
  const uint8_t XP3Mark2[] =
		{ 0x8b, 0x67, 0x01, 0xff/* sentinel */ };
  uint8_t XP3Mark[11];//for comparing
  bool DoInit = true;
  uint64_t offset;

  if(DoInit){
 		// the XP3 header above is splitted into two part; to avoid
		// mis-finding of the header in the program's initialized data area.
		DoInit = false;
		memcpy(XP3Mark, XP3Mark1, 8);
		memcpy(XP3Mark + 8, XP3Mark2, 3);
		// here joins it.
    mark[0] = 0; // sentinel
  }

  filest = fopen(filepath, "rb"); // open XP3 file

  if (NULL == filest)
  {
    gowrong("Cannot read file!");
  } // exit when readfile_fail

  fread(&mark, sizeof(mark), 1, filest); //read magic

  if(memcmp(XP3Mark, mark, sizeof(mark))==0){
    printf("Found a XP3 archive\n");
  }
  else
  {
    gowrong("not a valid XP3 archive!");
  }
  
  printf("Trying to read XP3 virtual file system information from : %s\n",filepath);
  //XP3DataHeader.OriginalSize = XP3Header.offset;

  if (XP3DataHeader.bZlib & TVP_XP3_INDEX_ENCODE_METHOD_MASK == TVP_XP3_INDEX_ENCODE_ZLIB)
  {
    printf("File context table were compressed\n");
  }
  do
  {
    offset = XP3DataHeader.OriginalSize + SEEK_SET;
    fseek(filest, offset, SEEK_SET);
    fread(&XP3DataHeader, sizeof(XP3DataHeader), 1, filest);
    printf("\nhp:%x\tlp:%x\niscped:%x\traw_cp:%d\n",
           i64highpart(XP3DataHeader.ArchiveSize),
           i64lowpart(XP3DataHeader.ArchiveSize),
           (XP3DataHeader.bZlib) & (0x80),
           XP3DataHeader.bZlib); // print some info
  } while (XP3DataHeader.bZlib & 0x80);

  fclose(filest);
  return EXIT_SUCCESS;
}

int64_t getheaderinXP3(FILE *filest){
  fseek(filest, 0, SEEK_SET);
  return 0;//no need to find offset in pure XP3
}

int64_t getheaderinPE(FILE *filest){
  fseek(filest, 0, SEEK_SET);
  return 0;
}

int main(int argc, char *argv[])
{
  if (argc == 1)
  {
    help();
  }
  else
  {
    if (isfile(argv[FIRST]))
    {
      getxp3info(argv[FIRST]);
    }
  }
}
