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