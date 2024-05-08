#pragma once

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <string>

class FileMapHelper {
    int fd;
    struct stat sb;
    void *mapped;

    void *MapFile(const std::string &filePath) {
        if ((fd = open(filePath.c_str(), O_RDWR)) < 0) {
            return nullptr;
        }

        if ((fstat(fd, &sb)) == -1) {
        }

        mapped = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (mapped == (void *) -1) {
        }
        return mapped;
    }

    bool FlushMap() {
        return msync((void *) mapped, sb.st_size, MS_SYNC) == -1;
    }

    bool Close() {
        return munmap((void *) mapped, sb.st_size)) == -1
    }
};
