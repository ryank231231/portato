#include <stdio.h>
#include <stdbool.h>

/*porting mingw */
typedef unsigned char BYTE;
typedef void HANDLE;
typedef long long LARGE_INTEGER;
typedef wchar_t WCHAR;
typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef WCHAR *PWCHAR;
typedef void *PVOID;

typedef struct
{
    long Encrypt;
    long Compress;
    WCHAR FileName[FILENAME_MAX];
} PACK_FILE_INFO;

typedef struct
{
    BYTE Magic[0xB];
    LARGE_INTEGER IndexOffset;
} KRKR2_XP3_HEADER;

typedef struct
{
    BYTE bZlib;
    LARGE_INTEGER ArchiveSize;
    LARGE_INTEGER OriginalSize;
} KRKR2_XP3_DATA_HEADER;

typedef struct
{
    ULONG Magic; // 'File'
    LARGE_INTEGER ChunkSize;
} KRKR2_XP3_INDEX_CHUNK_FILE;

typedef struct
{
    ULONG Magic; // 'info'
    LARGE_INTEGER ChunkSize;
    ULONG EncryptedFlag;
    LARGE_INTEGER OriginalSize;
    LARGE_INTEGER ArchiveSize;
    USHORT FileNameLength;
    PWCHAR FileName;
} KRKR2_XP3_INDEX_CHUNK_INFO2;
typedef struct
{
    ULONG Magic; // 'segm'
    LARGE_INTEGER ChunkSize;
    KRKR2_XP3_INDEX_CHUNK_SEGM_DATA segm[1];
} KRKR2_XP3_INDEX_CHUNK_SEGM;
typedef struct
{
    bool bZlib; // bZlib & 7 -> 1: compressed  0: raw  other: error
    LARGE_INTEGER Offset;
    LARGE_INTEGER OriginalSize;
    LARGE_INTEGER ArchiveSize;
} KRKR2_XP3_INDEX_CHUNK_SEGM_DATA;

typedef struct
{
    ULONG Magic; // 'adlr'
    LARGE_INTEGER ChunkSize;
    ULONG Hash;
} KRKR2_XP3_INDEX_CHUNK_ADLR;

typedef struct
{
    KRKR2_XP3_INDEX_CHUNK_FILE file;
    KRKR2_XP3_INDEX_CHUNK_INFO2 info;
    KRKR2_XP3_INDEX_CHUNK_SEGM segm;
    KRKR2_XP3_INDEX_CHUNK_ADLR adlr;
} SMyXP3Index;

/*
文件开头是文件标志，为"XP3"
偏移11处是文件信息表的位置，uint64
从上面的值指示跳到文件信息表处，有以下一个结构：
struct    sXP3Info
{
    byte_t    zlib; // 文件信息表是否用zlib压缩过
    uint64    psize; // 文件信息表在包文件中的大小
#if zlib
    uint64    rsize; // 文件信息表解压后的大小
#endif
    byte_t    fileInfo[psize]; // 文件信息表数据
};
成功获取文件信息表数据后可以得到以下一组结构的数组，用于描述包中文件的信息：
struct    sXP3File
{
    uint32    tag1; // 标志1，"File" 0x656c6946
    uint64    fileSize; // 文件信息数据大小
    uint32    tag2; // 标志2，"info" 0x6f666e69
    uint64    infoSize; // 文件基本数据大小
    uint32    protect; // 估计是表示此文件是否加过密
    uint64    rsize; // 文件原始大小
    uint64    psize; // 文件包中大小
    uint16    nameLen; // 文件名长度（指的是UTF-16字符个数）
    wchar_t fileName[nameLen]; // 文件名(UTF-16LE编码，无0结尾)
    uint32    tag3; // 标志3，"segm" 0x6d676573
    uint64    segmSize; // 文件段数据大小
    uint32    compress; // 文件是否用zlib压缩过
    uint64    offset; // 文件开始的位置
    uint64    rsize;
    uint64    psize;
#if fileSize - infoSize - segmSize - 24 > 0
    uint32    tag4; // 标志4，"adlr" 0x726c6461
    uint64    adlrSize; // 文件附加数据大小，一般是4
    uint32    key; // 附加数据，用于加密
#endif
};
*/