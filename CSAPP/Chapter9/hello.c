//
// Created by along on 17-12-28.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

int main() {
    int fd;
    char *buf = NULL;
    struct stat stat;
    const char *path = "hello.txt";

    // 使用文件描述符
    fd = open(path, O_RDWR | O_CLOEXEC, 0);
    if (fd < 0) {
        perror("open()");
        exit(EXIT_FAILURE);
    }
    fstat(fd, &stat);

    // 使用mmap函数将文件映射到内存中
    buf = mmap(NULL, (size_t) stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    buf[0] = 'J';
    munmap(buf, (size_t) stat.st_size);
    close(fd);
    exit(EXIT_SUCCESS);
}