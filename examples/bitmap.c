#include <stdio.h>
#include <SDL2/SDL.h>

#define BITMAP_IMPLEMENTATION
#include "../bitmap.h"

static int create_texture4(SDL_Texture* tex, bitmap_t* bmp);
static int create_texture8(SDL_Texture* tex, bitmap_t* bmp);
static int create_texture16(SDL_Texture* tex, bitmap_t* bmp);
static int create_texture24(SDL_Texture* tex, bitmap_t* bmp);

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: ./bitmap [filename]\n");
        return 0;
    }
    bitmap_t bmp;
    if (bitmap_load_file(argv[1], &bmp) < 0) return 0;
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("bitmap", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 380, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Event event;

    int width, height;
    width = bmp.info_header.width;
    height = bmp.info_header.height;

    SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height);
    switch (bmp.info_header.bits_per_pixel) {
        case 4: create_texture4(tex, &bmp); break;
        case 8: create_texture8(tex, &bmp); break;
        case 16: create_texture16(tex, &bmp); break;
        case 24: create_texture24(tex, &bmp); break;
        default:
            fprintf(stderr, "invalid bits per pixel\n");
            goto finish;
    }

    SDL_Rect dest = { 0, 0, width*2, height*2 };

    while (event.type != SDL_QUIT) {
        while (SDL_PollEvent(&event));
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderCopy(renderer, tex, NULL, &dest);
        SDL_RenderPresent(renderer);
    }

    printf("saindo do loop\n");

finish:

    bitmap_free(&bmp);
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
/*
16 bits
r 0x1111100000000000
g 0x0000011111100000
b 0x0000000000011111
------- masks -------
0xf800  0x7e0  0x1f
*/
int create_texture16(SDL_Texture* tex, bitmap_t* bmp) {
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
        Uint16 c = *((Uint16*)(bmp->pixels + (i*2)));
        Uint8 b = ((float)(c & 0x1f) / 32.f) * 255;
        Uint8 g = ((float)((c & 0x7e0) >> 5) / 32.f) * 255;
        Uint8 r = ((float)((c & 0xf800) >> 11) / 32.f) *255;
        Uint32 cc = 0xFF000000 | (b << 16) | (g << 8) | r;
        pixels[i] = cc;
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
        Uint32 cc = 0xFF000000 | (c[0] << 16) | (c[1] << 8) | c[2];
        pixels[i] = cc;
    }
    SDL_UnlockTexture(tex);
    return 0;
}