#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

typedef struct {
    unsigned short type;
    unsigned int size;
    unsigned short reserved0;
    unsigned short reserved1;
    unsigned int offset;
} __attribute__((__packed__)) file_header_t;
    
typedef struct {
    unsigned int size;
    int width;
    int height;
    unsigned short planes;
    unsigned short bits_per_pixel;
    unsigned int compression;
    unsigned int image_size;
    int x_resolution;
    int y_resolution;
    unsigned int colors_used;
    unsigned int colors_important;
} __attribute__((__packed__)) info_header_t;

typedef struct {
    file_header_t file_header;
    info_header_t info_header;
    unsigned int* palette;
    unsigned char* pixels;
} bitmap_t;

static void read_headers(FILE* fp, file_header_t* fh, info_header_t* fi);

static bitmap_t load_bitmap(FILE* fp);
static bitmap_t create_bitmap(int width, int height, int bpp);

static void save_bitmap(const char* filename, bitmap_t* bmp);

static void free_bitmap(bitmap_t* bmp);

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stdout, "Usage: ./bitmap [img.bmp]\n");
        return -1;
    }
    FILE* fp;
    if (!(fp = fopen(argv[1], "rb"))) {
        fprintf(stderr, "Cannot open %s\n", argv[1]);
        return -1;
    }
    bitmap_t bmp = load_bitmap(fp);
    fclose(fp);
    if (!bmp.pixels) {
        fprintf(stderr, "Failed to load image\n");
        return -1;
    }
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("bitmap", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 380, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Event event;

    SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, bmp.info_header.width, bmp.info_header.height);

    int pitch;
    void* data;
    SDL_LockTexture(tex, NULL, &data, &pitch);
    Uint32* pixels = (Uint32*)data;
    int size = bmp.info_header.width * bmp.info_header.height;
    for (int i = 0; i < size; i++) {
        Uint32 c = 0xFF000000;
        Uint8* p = bmp.pixels + (i * 3);
        c = 0xFF000000 | (p[0] << 16) |
            (p[1] << 8) | p[2];
        pixels[i] = c;
    }
    SDL_UnlockTexture(tex);

    while (event.type != SDL_QUIT) {
        while (SDL_PollEvent(&event));
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderCopy(renderer, tex, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    free_bitmap(&bmp);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

bitmap_t load_bitmap(FILE* fp) {
    bitmap_t bmp = {0};
    if (!fp) return bmp;
    file_header_t* fh = &(bmp.file_header);
    info_header_t* ih = &(bmp.info_header);

    fread(fh, 1, sizeof(file_header_t), fp);
    if (fh->type != 0x4D42) {
        fprintf(stderr, "File is not a valid bitmap\n");
        return (bitmap_t){0};
    }
    fread(ih, 1, sizeof(info_header_t), fp);

    float size = (float)ih->bits_per_pixel / 8.f;
    int pitch = ih->width * size;
    int data_size = ih->width * pitch;

    int pal_size = fh->offset - 54;
    bmp.pixels = malloc(data_size);
    if (pal_size > 0) bmp.palette = malloc(pal_size);
    /*
     * As paletas são guardadas no format BGR, então é necessário fazer um swap para utilizar
    */
    for (int i = 0; i < (pal_size / 4); i++) {
        char c[4];
        fread(c, 1, sizeof(c), fp);
        char aux = c[0];
        c[0] = c[2];
        c[2] = aux;
        unsigned int* swap = (unsigned int*)c;
        bmp.palette[i] = *swap;
    }

    fseek(fp, fh->offset, SEEK_SET);

    /**
     * Os pixels guardados como se o eixo Y estivesse invertido, então é necessário ler linha por linha, porém de baixo para cima
    */
    for (int y = 0; y < ih->height; y++) fread(bmp.pixels + ((ih->height-1-y)*pitch), 1, pitch, fp);
    return bmp;
}

bitmap_t create_bitmap(int width, int height, int bpp) {
    bitmap_t bmp = (bitmap_t){0};
    float size = (float)bpp / 8.f;
    memset(&bmp, 0, sizeof(bitmap_t));
    file_header_t* fh = (file_header_t*)&(bmp.file_header);
    fh->type = 0x4D42;
    fh->size = sizeof(file_header_t) + sizeof(info_header_t) + (width*height*size);
    fh->offset = sizeof(file_header_t) + sizeof(info_header_t);

    info_header_t* ih = (info_header_t*)&(bmp.info_header);
    ih->size = 40;
    ih->width = width;
    ih->height = height;
    ih->planes = 1;
    ih->bits_per_pixel = bpp;
    ih->compression = 0;
    ih->image_size = width * height * size;
    ih->x_resolution = 0; 
    ih->y_resolution = 0;
    ih->colors_used = 0;
    ih->colors_important = 0;

    bmp.pixels = malloc(ih->image_size);

    return bmp;
}

void save_bitmap(const char* filename, bitmap_t* bmp) {
    if (!filename || !bmp) return;
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "failed to create %s\n", filename);
        return;
    }
    fwrite(&(bmp->file_header), 1, sizeof(file_header_t), fp);
    fwrite(&(bmp->info_header), 1, sizeof(info_header_t), fp);

    int width = bmp->info_header.width;
    int height = bmp->info_header.height;
    int bpp = bmp->info_header.bits_per_pixel;

    /**
     * Transforma a paleta de volta para BGR
    */
    if (bpp <= 8) {
        int colors = (bmp->file_header.offset - 54) / sizeof(int);
        for (int i = 0; i < colors; i++) {
            unsigned int c = bmp->palette[i];
            unsigned char* cc = (unsigned char*)&c;
            char aux = cc[0];
            cc[0] = cc[2];
            cc[2] = aux;
            fwrite(cc, 1, sizeof(int), fp);
        }
    }

    /**
     * Salva a imagem com o eixo Y invertido
    */
    float sz = (float)bmp->info_header.bits_per_pixel / 8.f;
    int pitch = width * sz;
    for (int y = 0; y < height; y++) fwrite(bmp->pixels + (height-1-y)*pitch, 1, pitch, fp);
    fclose(fp);
}

void free_bitmap(bitmap_t* bmp) {
    if (bmp->palette) free(bmp->palette);
    if (bmp->pixels) free(bmp->pixels);
    bmp->palette = NULL;
    bmp->pixels = NULL;
}