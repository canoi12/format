#ifndef _BITMAP_H_
#define _BITMAP_H_

#define BMP_API

typedef struct bitmap_t bitmap_t;
typedef struct bitmap_file_header_t bitmap_file_header_t;
typedef struct bitmap_info_header_t bitmap_info_header_t;;

/* 14 bytes */
struct __attribute__((__packed__)) bitmap_file_header_t {
    unsigned short type; // geralmente 0x4D42 ou 'BM', serve somente para validar o arquivo como bitmap
    unsigned int size; // tamanho dos cabeçalhos + tamanho da imagem em bytes
    unsigned short reserved0;
    unsigned short reserved1;
    unsigned int offset; // a partir de onde começa os dados de pixel no arquivo
};

/* 40 bytes */
struct __attribute__((__packed__)) bitmap_info_header_t {
    unsigned int size; // tamanho dos dados no info, pode variar caso a imagem tenha uma paleta de cores salva no arquivo
    int width; // largura em pixels
    int height; // altura em pixels
    unsigned short planes; // nom sei
    unsigned short bits_per_pixel; // quantidade de bits usadas pra armazenar um pixel
    unsigned int compression; // tipo de compressão utilizada, geralmente é 0
    unsigned int image_size; // tamanho da imagem em bytes
    int x_resolution; // nom sei
    int y_resolution; // nom sei
    unsigned int colors_used; // quantidade de cores da paleta utilizada, é necessário somente se a quantidade de bits por pixel for <= 8
    unsigned int colors_important;
};

struct bitmap_t {
    bitmap_file_header_t file_header;
    bitmap_info_header_t info_header;
    unsigned int* palette;
    unsigned char* pixels;
};

BMP_API void bitmap_read_headers(const char* filename, bitmap_file_header_t* file_header, bitmap_info_header_t* info_header);

BMP_API int bitmap_load_file(const char* filename, bitmap_t* out);
BMP_API int bitmap_write_file(const char* filename, bitmap_t* bmp);

BMP_API void bitmap_free(bitmap_t* bmp);

#endif /* _BITMAP_H_ */

#if defined(BITMAP_IMPLEMENTATION)

void bitmap_read_headers(const char* filename, bitmap_file_header_t* file_header, bitmap_info_header_t* info_header) {
    if (!file_header) return;
    FILE* fp;
    fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "failed to open bitmap: %s\n", filename);
        return;
    }
    fread(file_header, 1, sizeof(bitmap_file_header_t), fp);
    if (file_header->type != 0x4D42) {
        fprintf(stderr, "invalid bitmap file\n");
        return;
    }
    if (info_header) fread(info_header, 1, sizeof(bitmap_info_header_t), fp);
    fclose(fp);
}

int bitmap_load_file(const char* filename, bitmap_t* out) {
    if (!filename || !out) return -1;
    memset(out, 0, sizeof(*out));
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "failed to load file: %s\n", filename);
        return -1;
    }
    bitmap_file_header_t* fhead = &(out->file_header);
    bitmap_info_header_t* ihead = &(out->info_header);
    fread((void*)fhead, 1, sizeof(*fhead), fp);
    if (fhead->type != 0x4D42) {
        fprintf(stderr, "invalid bitmap file: %s\n", filename);
        return -1;
    }
    fread((void*)ihead, 1, sizeof(*ihead), fp);
    float size = (float)ihead->bits_per_pixel / 8.f;
    int pitch = ihead->width * size;
    int data_size = ihead->width * pitch;

    /*
     * aqui eu descobri que essa foi a maneira que deu mais certo
     * pra calcular o tamanho da paleta da minha imagem, antes eu tava
     * utilizando a 'colors_used', mas vi que nem todas as imagens tem
     * as todas as informações certinhas no cabeçalho
     * 
     * a 'img.bmp' é um exemplo disso, não tem informação de 'colors_used';
     * 
     * mas é isso, aqui acho que tem N maneiras de se fazer isso
    */
    int pal_size = (fhead->offset - 54);
    out->pixels = malloc(data_size);
    if (pal_size > 0) out->palette = malloc(pal_size);
    /*
     * descobri enquanto testava que os pixels das palettas
     * estão em BGR ao invés de RGB, então foi necessário fazer esse 
     * swap
    */
    for (int i = 0; i < (pal_size / 4); i++) {
        char c[4];
        fread(c, 1, sizeof(c), fp);
        char aux = c[0];
        c[0] = c[2];
        c[2] = aux;
        Uint32* swap = (Uint32*)c;
        out->palette[i] = *swap;
    }
    // fread(out->palette, 1, pal_size, fp);

    // char *buffer = malloc(data_size);

    /* mudo a posição do arquivo pro começo dos pixels da imagem */
    fseek(fp, fhead->offset, SEEK_SET);

    // fread(buffer, 1, data_size, fp);
    /*
     * mesma coisa aqui, só descobri depois dos testes que por algum
     * motivo os pixels são lidos do final pro começo
     *
     * mas somente o eixo Y é invertido, o X tu lê a linha normalmente
    */
    for (int y = 0; y < ihead->height; y++) fread(out->pixels + ((ihead->height-1-y)*pitch), 1, pitch, fp);

    // free(buffer);

    fclose(fp);

    return 0;
}

/*
 * a função de escrita é bem tranquila já que o cabeçalho já está num formato legível
 * a única coisa que precisa ser feita é reverter os processos que foram feitos
 * na função de load (voltar as paletas pra BGR e escrever os pixels da imagem de baixo pra cima)
 */
int bitmap_write_file(const char* filename, bitmap_t* bmp) {
    if (!filename || !bmp) return -1;
    FILE *fp;
    fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "failed to open file for write: %s\n", filename);
        return -1;
    }
    int width = bmp->info_header.width;
    int height = bmp->info_header.height;
    /* só escrever os cabeçalhos direto */
    fwrite(&(bmp->file_header), 1, sizeof(bitmap_file_header_t), fp);
    fwrite(&(bmp->info_header), 1, sizeof(bitmap_info_header_t), fp);
    int bpp = bmp->info_header.bits_per_pixel;

    /* literalmente o mesmo processo de swap */
    if (bpp <= 8) {
        int colors = (bmp->file_header.offset - 54) / sizeof(Uint32);
        for (int i = 0; i < colors; i++) {
            Uint32 c = bmp->palette[i];
            Uint8* cc = (Uint8*)&c;
            Sint8 aux = cc[0];
            cc[0] = cc[2];
            cc[2] = aux;
            fwrite(cc, 1, sizeof(int), fp);
        }
    }

    /* e aqui também a mesma coisa, invertendo os pixels em Y */
    float sz = (float)bmp->info_header.bits_per_pixel / 8.f;
    int pitch = width * sz;
    for (int y = 0; y < height; y++) fwrite(bmp->pixels + (height-1-y)*pitch, 1, pitch, fp);
    fclose(fp);
    return 0;
}

void bitmap_free(bitmap_t* bmp) {
    if (!bmp) return;
    if (bmp->palette) free(bmp->palette);
    if (bmp->pixels) free(bmp->pixels);
    bmp->palette = NULL;
    bmp->pixels = NULL;
}

#endif /* BITMAP_IMPLEMENTATION */