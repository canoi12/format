#ifndef _TAR_H_
#define _TAR_H_

typedef struct tar_t tar_t;
typedef struct tar_header_t tar_header_t;

/*
 * O formato tar tem um cabeçalho de 512 bytes, e todo arquivos é lido em blocos do mesmo tamanho
 *
 *  -----------------
 * |                 |
 * |      header     | 512 bytes
 * |                 |
 *  -----------------
 * |                 |
 * |    file data    | 512 bytes
 * |                 |
 *  -----------------
 * |                 |
 * |      header     | 512 bytes
 * |                 |
 *  -----------------
 * |                 |
 * |    file data    | 512 bytes
 * |                 |
 *  -----------------
 *          .
 *          .
 *          .
 * 
 */
struct tar_header_t {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char _[12];
};

struct tar_t {
    tar_header_t header;
    int size;
    void* ptr;
}


#endif /* _TAR_H_ */

#if defined(TAR_IMPLEMENTATION)

#endif /* TAR_IMPLEMENTATION */