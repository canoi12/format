#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
    int8_t id[4];
    uint32_t chunk_size;
    int8_t format[4];
    int8_t subchunk1_id[4];
    uint32_t subchunk1_size;
    uint16_t audio_format;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    int8_t subchunk2_id[4];
    uint32_t subchunk2_size;
} wav_header_t;

typedef struct {
    wav_header_t header;
    int data_size;
    void* data;
} wav_t;

static wav_t load_wav(FILE* fp);
static wav_t create_wav(int sample_rate, int channels, int duration);

static void save_wav(const char* filename, wav_t* wav);

static void free_wav(wav_t* wav);

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stdout, "Usage: ./wav [music.wav]\n");
        return 0;
    }
    FILE* fp;
    if (!(fp = fopen(argv[1], "rb"))) {
        fprintf(stderr, "cannot open %s\n", argv[1]);
        return 0;
    }
    wav_t wav = load_wav(fp);
    fclose(fp);
    free_wav(&wav);
    return 0;
}

wav_t load_wav(FILE* fp) {
    wav_t wav = (wav_t){0};
    if (!fp) return wav;
    memset(&wav, 0, sizeof(wav_t));
    fread(&(wav.header), 1, sizeof(wav_header_t), fp);
    char* id = wav.header.id;
    if (id[0] != 'R' || id[1] != 'I' ||
        id[2] != 'F' || id[2] != 'F') {
            fprintf(stderr, "file is not a valid wav\n");
            return (wav_t){0};
        }
    wav.data_size = wav.header.subchunk2_size;
    wav.data = malloc(wav.data_size);
    fread(wav.data, 1, wav.data_size, fp);
    return wav;
}

void free_wav(wav_t* wav) {
    if (!wav) return;
    if (wav->data) free(wav->data);
    wav->data = NULL;
}