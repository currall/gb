#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// read any number of bytes from file

int read_data(int fd, void* buf, int pos, int bytesToRead){
    if (fd < 0){
        perror("invalid file descriptor");
        return 0;
    }

    if ((lseek(fd,pos,SEEK_SET)) == -1){
        perror("error seeking in file");
        close(fd);
        return 0;
    }

    int bytesRead = read(fd, buf, bytesToRead);
    if (bytesRead < 0) {
        perror("error reading data");
        close(fd);
        return 0;
    }

    if (bytesRead != bytesToRead) {
        // EOF reached – not an error!
        printf("WARNING: read %d / %d (EOF)\n", bytesRead, bytesToRead);
        close(fd);
        return 0;
    }
    return 1;
}

int read_file(char* file, void* buf, int pos, int bytesToRead){
    int fd = open(file, O_RDONLY | O_BINARY);
    read_data(fd,buf,pos,bytesToRead);
    close(fd);
    return 1;
}

uint32_t load_rom(char* file, uint8_t** m) {
    FILE* f = fopen(file, "rb");
    if (!f) {
        perror("Failed to open ROM");
        return 0;
    }

    // file size
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size <= 0) {
        printf("ROM file is empty or invalid\n");
        fclose(f);
        return 0;
    }

    uint32_t rom_size = (uint32_t)size;

    *m = (uint8_t*)malloc(rom_size);
    if (!*m) {
        perror("Failed to allocate ROM memory");
        fclose(f);
        return 0;
    }

    // read data
    size_t read_count = fread(*m, 1, rom_size, f);
    fclose(f);

    if (read_count != rom_size) {
        printf("Error: Read partial ROM (Expected %u, got %zu)\n", rom_size, read_count);
        free(*m);
        *m = NULL;
        return 0;
    }

    printf("[MEMORY] Loaded ROM: %s (%u bytes)\n", file, rom_size);
    return rom_size;
}