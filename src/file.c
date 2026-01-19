#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

// read any number of bytes from file
int read_data(char* file, void* buf, int pos, int bytesToRead){
    int fd = open(file, O_RDONLY | O_BINARY);
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

    close(fd);
    return 1;
}