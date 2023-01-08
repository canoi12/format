#ifndef _WAV_H_
#define _WAV_H_

#include <stdio.h>
#include <stdlib.h>

#define WAV_API

typedef struct wav_t wav_t;
typedef struct wav_header_t wav_header_t;

struct wav_header_t {
    char id[4];
    unsigned int chunk_size;
    char format[4];
    char subchunk1_id[4];
    unsigned int subchunk1_size;
    unsigned short audio_format;
    unsigned short channels;
    unsigned int sample_rate;
    unsigned int byte_rate;
    unsigned short block_align;
    unsigned short bits_per_sample;
    char subchunk2_id[4];
    unsigned int subchunk2_size;
};

struct wav_t {
    wav_header_t header;
    int data_size;
    void* data;
};

WAV_API void wav_read_header(const char* filename, wav_header_t* out);

WAV_API int wav_load_file(const char* filename, wav_t* out);
WAV_API void wav_write_file(const char* filename, wav_t* wav);

#endif /* _WAV_H_ */

#if defined(WAV_IMPLEMENTATION)

int wav_load_file(const char* filename, wav_t* out) {
    if (!filename || !out) return -1;
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "failed to load wav file: %s\n", filename);
        return -1;
    }
    fread(&(out->header), 1, sizeof(wav_header_t), fp);
    char* id = out->header.id;
    if (id[0] != 'R' || id[1] != 'I' ||
        id[2] != 'F' || id[3] != 'F') {
            fprintf(stderr, "invalid wav file: %s\n", filename);
            return -1;
        }
    out->data = malloc(out->header.subchunk2_size);
    out->data_size = out->header.subchunk2_size;
    fread(out->data, 1, out->header.subchunk2_size, fp);
    fclose(fp);

    return 0;
}

#endif /* WAV_IMPLEMENTATION */