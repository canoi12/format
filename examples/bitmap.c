#include <stdio.h>
#include <SDL2/SDL.h>

#define BITMAP_IMPLEMENTATION
#include "../bitmap.h"

static int create_texture4(SDL_Texture* tex, bitmap_t* bmp);
static int create_texture8(SDL_Texture* tex, bitmap_t* bmp);
// static int create_texture16(SDL_Texture* tex, bitmap_t* bmp);
static int create_texture24(SDL_Texture* tex, bitmap_t* bmp);

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: ./bitmap [filename]\n");
        return 0;
    }
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("bitmap", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 380, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Event event;

    bitmap_t bmp;
    if (bitmap_load_file(argv[1], &bmp) < 0)
        goto finish;

    int width, height;
    width = bmp.info_header.width;
    height = bmp.info_header.height;
    float sz = (float)bmp.info_header.bits_per_pixel / 8.f;

    int pitch = height * sz;
    // char *buffer = malloc(width * pitch);
    // memcpy(buffer, bmp.pixels, width * pitch);
    // for (int y = 0; y < height; y++) {
    //     memcpy(bmp.pixels + (y * pitch), buffer + ((height-y-1) * pitch), pitch);
    // }
    SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height);
    switch (bmp.info_header.bits_per_pixel) {
        case 4: create_texture4(tex, &bmp); break;
        case 8: create_texture8(tex, &bmp); break;
        case 24: create_texture24(tex, &bmp); break;
    }
    #if 0
    SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height);
    void* data;
    SDL_LockTexture(tex, NULL, &data, &pitch);
    int size = width * height;
    Uint32* pixels = data;
    for (int i = 0; i < size; i += 2) {
        unsigned char c = bmp.pixels[i/2];
        int p = (c & 0xF0) >> 4;
        pixels[i] = bmp.palette[p] | 0xFF000000;
        p = c & 0xF;
        pixels[i+1] = bmp.palette[p] | 0xFF000000;
        // pixels[i] = 0xFF0000FF;
        // pixels[i+1] = 0xFF00FF00;
    }
    SDL_UnlockTexture(tex);
    #elseif 0
    SDL_Surface* surf = SDL_CreateRGBSurfaceFrom(bmp.pixels, width, height, bmp.info_header.bits_per_pixel, pitch, 0, 0, 0, 0);
    // free(buffer);
    
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    printf("testando\n");
    SDL_FreeSurface(surf);
    #endif
    #if 0
    Uint32* pixels = data;
    for (int i = 0; i < size; i++) {
        pixels[i] = 0xff000000;
        int index = i*3;
        // Uint32 c = *((Uint32*)(bmp.pixels + index)) >> 8;
        pixels[i] = 0xff000000 | (bmp.pixels[index] << 16) | (bmp.pixels[index+1] << 8) | bmp.pixels[index+2];
        // pixels[i] = 0xff000000 | c;
    }
    #endif

    SDL_Rect dest = { 0, 0, width*2, height*2 };

    while (event.type != SDL_QUIT) {
        while (SDL_PollEvent(&event));
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderCopy(renderer, tex, NULL, &dest);
        SDL_RenderPresent(renderer);
    }

    bitmap_write_file("bora.bmp", &bmp);
    free(bmp.pixels);
    if (bmp.palette) free(bmp.palette);
    printf("saindo do loop\n");

finish:

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

int create_texture4(SDL_Texture* tex, bitmap_t* bmp) {
    if (!tex || !bmp) return -1;
    void* data;
    int pitch;
    SDL_LockTexture(tex, NULL, &data, &pitch);
    Uint32* pixels = data;
    int width, height;
    width = bmp->info_header.width;
    height = bmp->info_header.height;
    int size = width * height;
    for (int i = 0; i < size; i += 2) {
        Uint8 c = bmp->pixels[i/2];
        int p = (c & 0xF0) >> 4;
        pixels[i] = bmp->palette[p] | 0xFF000000;
        p = c & 0xF;
        pixels[i+1] = bmp->palette[p] | 0xFF000000;
    }
    SDL_UnlockTexture(tex);
    return 0;
}

int create_texture8(SDL_Texture* tex, bitmap_t* bmp) {
    if (!tex || !bmp) return -1;
    void* data;
    int pitch;
    SDL_LockTexture(tex, NULL, &data, &pitch);
    Uint32* pixels = data;
    int width, height;
    width = bmp->info_header.width;
    height = bmp->info_header.height;
    int size = width * height;
    for (int i = 0; i < size; i++) {
        Uint8 c = bmp->pixels[i];
        pixels[i] = bmp->palette[c] | 0xFF000000;
    }
    SDL_UnlockTexture(tex);
    return 0;
}

int create_texture24(SDL_Texture* tex, bitmap_t* bmp) {
    if (!tex || !bmp) return -1;
    void* data;
    int pitch;
    SDL_LockTexture(tex, NULL, &data, &pitch);
    Uint32* pixels = data;
    int width, height;
    width = bmp->info_header.width;
    height = bmp->info_header.height;
    int size = width * height;
    for (int i = 0; i < size; i++) {
        Uint8* c = bmp->pixels + (i*3);
        int ii = i*3;
        pixels[i] = 0xFF000000 | c[ii] | (c[ii+1] << 8) | (c[ii+2] << 16);
    }
    SDL_UnlockTexture(tex);
    return 0;
}