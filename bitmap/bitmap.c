#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char** argv) {
    return 0;
}

bitmap_t load_bitmap(FILE* fp) {
    bitmap_t bmp = {0};
    if (!fp) return bmp;

    
}