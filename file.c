#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

// read any number of bytes from file
int readData(char* file, void* buf, int pos, int bytesToRead){
    int fd = open(file, O_RDONLY);
    if (fd < 0){
        perror("invalid file descriptor");
        return 0;
    }

    if ((lseek(fd,pos,SEEK_SET)) == -1){
        perror("error seeking in file");
        return 0;
    }

    int bytesRead = read(fd,buf,bytesToRead);
    if (bytesRead != bytesToRead) {
        perror("error reading data");
        return 0;
    }

    close(fd);
    return 1;
}