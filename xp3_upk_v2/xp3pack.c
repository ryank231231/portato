#include <stdio.h>
#include <sys/stat.h>

#define FIRST 1

int isfile(char path[])
{
    struct _stat buf;

    int result;
    result = _stat(&path[0], &buf);
    if (_S_IFDIR & buf.st_mode)
    {
        //printf("folder\n");
        return 1;
    }
    else if (_S_IFREG & buf.st_mode)
    {
        //printf("file\n");
        return 2;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int filepath_enum = 0;
    char filepath[filepath_enum];
    int filec_enum = 0;
    int rubbish = 0xFF;

    if (argc == 1)
    {
        return 0; //exit when no args
    }
    else
    {
        //printf("args detected\n");
        for (argc = argc - 1; argc; --argc) //enum given file/folder
        {
            filec_enum++;
            //printf("%s", argv[filec_enum]);
            //printf("\n");
            //printf("%d\n", filec_enum);
        };
        switch (isfile(argv[FIRST]))
        {
        case 1:
            printf("it's a folder\n");
            break;
        case 2:
            printf("it's a file\n");
            break;
            default:
            printf("ERROR HAPPENED\n");
            break;
        }
    }
}
