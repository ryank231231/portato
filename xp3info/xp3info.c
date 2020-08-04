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
  FILE *xp3file = NULL;
  struct KRKR2_XP3Header XP3Header;
  struct KRKR2_XP3_DATA_HEADER XP3DataHeader;
  struct KRKR2_XP3_INDEX XP3Index;

  xp3file = fopen(filepath, "rb"); // open XP3 file

  if (NULL == xp3file)
  {
    gowrong("Cannot read file!");
  } // exit when file_fail

  fseek(xp3file, 0, SEEK_SET);
  fread(&XP3Header.Magic, sizeof(XP3Header.Magic), 1, xp3file); //read magic
  if (i32conv(XP3Header.Magic) != XP3_HEADER_MAGIC)
  {
    gowrong("not a valid XP3 archive!");
  }
  printf("Found a XP3 archive\n");
  XP3DataHeader.OriginalSize = XP3Header.offset;
  int64_t offset;

  if (XP3DataHeader.bZlib & TVP_XP3_INDEX_ENCODE_METHOD_MASK == TVP_XP3_INDEX_ENCODE_ZLIB)
  {
    printf("File context table were compressed\n");
  }
  do
  {
    offset = XP3DataHeader.OriginalSize + SEEK_SET;
    fseek(xp3file, offset, SEEK_SET);
    fread(&XP3DataHeader, sizeof(XP3DataHeader), 1, xp3file);
    printf("\nhp:%x\tlp:%x\niscped:%x\traw_cp:%d\n",
           i64highpart(XP3DataHeader.ArchiveSize),
           i64lowpart(XP3DataHeader.ArchiveSize),
           (XP3DataHeader.bZlib) & (0x80),
           XP3DataHeader.bZlib); // print some info
  } while (XP3DataHeader.bZlib & 0x80);

  printf("Orig size:%lld\nComped size:%lld\nComp rate:%d%%\n", //doesn't work,returns error comped size.
         XP3DataHeader.OriginalSize, XP3DataHeader.ArchiveSize,
         (int)(XP3DataHeader.ArchiveSize / XP3DataHeader.OriginalSize));
  fclose(xp3file);
  return EXIT_SUCCESS;
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
