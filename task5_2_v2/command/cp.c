#include "stdio.h"
#include "string.h"

#define BUFFER_SIZE 256

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: cp <source> <destination>\n");
        return 1;
    }

    // 尝试以打开源文件
    int src_fd = open(argv[1], O_RDWR);
    if (src_fd < 0) {
        printf("Failed to open source file %s\n", argv[1]);
        return 1;
    }

    // 尝试打开目标文件，如果不存在则创建
    int dest_fd = open(argv[2], O_RDWR | O_CREAT);
    if (dest_fd < 0) {
        printf("Failed to open or create destination file %s\n", argv[2]);
        close(src_fd);
        return 1;
    }

    char buf[BUFFER_SIZE];
    int n;

    while ((n = read(src_fd, buf, BUFFER_SIZE)) > 0) {
        if (write(dest_fd, buf, n) != n) {
            printf("Failed to write to destination file\n");
            close(src_fd);
            close(dest_fd);
            return 1;
        }
    }

    close(src_fd);
    close(dest_fd);
    return 0;
}
