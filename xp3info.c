#include <stdio.h>
#include <sys/stat.h>

#define FIRST 1

int isfile(char path[])
{
    struct _stat buf;

    int result;
    result = _stat(&path[0], &buf);
    if (_S_IFREG & buf.st_mode)
    {
        return 1;//file
    }
    return 0;
}

int help(void){
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
            printf("it's a file\n");
        }
    }
}
