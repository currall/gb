#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t get_file_size(FILE* f) {
    // file size
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size <= 0) {
        printf("File is empty or invalid\n");
        fclose(f);
        return 0;
    }

    uint32_t file_size = (uint32_t)size;
}

uint32_t load_file(char* file, uint8_t** m) {
    FILE* f = fopen(file, "rb");
    if (!f) {
        perror("Failed to open file");
        return 0;
    }

    uint32_t file_size = get_file_size(f);

    *m = (uint8_t*)malloc(file_size);
    if (!*m) {
        perror("Failed to allocate memory");
        fclose(f);
        return 0;
    }

    // read data
    size_t read_count = fread(*m, 1, file_size, f);
    fclose(f);

    if (read_count != file_size) {
        printf("Error: Read partial file (Expected %d, got %d)\n", file_size, read_count);
        free(*m);
        *m = NULL;
        return 0;
    }

    printf("[FILE] Loaded file: %s (%d bytes)\n", file, file_size);
    return file_size;
}

char* savefile_name(char *file) {
    if (file == NULL) return NULL;

    size_t filename_len = strlen(file) + 5;
    char* savefile = (char *)malloc(filename_len);
    
    if (savefile == NULL) {
        return NULL; 
    }
    
    snprintf(savefile, filename_len, "%s.sav", file);
    return savefile;
}

void save_game(char* file, uint8_t* eram, uint32_t ram_size) {
    char *savefile = savefile_name(file);

    // open file
    FILE *fp = fopen(savefile, "wb");
    if (fp == NULL) {
        perror("[FILE] Error writing save file!");
        free(savefile);
        return;
    }

    // write eram to file
    uint32_t items_written = fwrite(eram, sizeof(uint8_t), ram_size, fp);

    // close file
    fclose(fp);
    free(savefile);
    printf("[FILE] Saved to file: %s (%d / %d bytes)\n", savefile, items_written , ram_size);

}

void load_game(char* file, uint8_t* eram, uint32_t ram_size) {
    if (file == NULL || eram == NULL) return;

    char *savefile = savefile_name(file);
    if (savefile == NULL) {
        return;
    }

    FILE *fp = fopen(savefile, "rb");
    if (fp == NULL) {
        perror("[FILE] No save file detected");
        free(savefile);
        return;
    }

    fread(eram, sizeof(uint8_t), ram_size, fp); // read data
    fclose(fp);
    free(savefile);

    printf("[FILE] Loaded save file: %s (%d bytes)\n", savefile, ram_size);

}