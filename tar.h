#ifndef _TAR_H_
#define _TAR_H_

#define TAR_API

typedef struct tar_t tar_t;
typedef struct tar_header_t tar_header_t;

/*
 * O formato TAR tem um cabeçalho de 512 bytes, e todo arquivos é lido em blocos do mesmo tamanho,
 * caso o arquivo tenha mais de 512 bytes, então novos blocos são alocados até conseguir conter todo o arquivo
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
 * |                 |
 * |    file data    | 512 bytes
 * |                 |
 *  -----------------
 *          .
 *          .
 *          .
 *
 * 
 * Como o TAR usa um header posix, então alguns conceitos vão fazer mais
 * sentido quando se está usando algum sistema UNIX based, como uid e gid
 * por exemplo
 * 
 */
struct tar_header_t {
    char name[100]; /* string| nome do arquivo */
    char mode[8]; /* string octal| modo do arquivo (rwx, (r)ead (w)rite e(x)ec)*/
    char uid[8]; /* string octal| uid do arquivo */
    char gid[8]; /* string octal| gid do arquivo */
    char size[12]; /* string octal| tamanho do arquivo */
    char mtime[12]; /* string octal| quando foi modificado pela ultima vez */
    char chksum[8]; /* string octal| checksum, que basicamente é a soma dos bytes do cabeçalho */
    char typeflag; /*  */
    char linkname[100]; /* string| caso seja um link simbólico, aqui guarda o caminho pra onde ele aponta */
    char magic[6]; /* string| 'ustar ' */
    char version[2]; /* */
    char uname[32]; /* string| nome do usuário */
    char gname[32]; /* string| nome do grupo */
    char devmajor[8]; /* */
    char devminor[8]; /* */
    char prefix[155]; /* */
    char _[12]; /* */
};

struct tar_t {
    tar_header_t header;
    int size;
    void* ptr;
}

TAR_API int tar_load_file(const char* filename, tar_t* out);
TAR_API int tar_write_file(const char* filename, tar_t* tar);

TAR_API void tar_free(tar_t* tar);


#endif /* _TAR_H_ */

#if defined(TAR_IMPLEMENTATION)

#endif /* TAR_IMPLEMENTATION */