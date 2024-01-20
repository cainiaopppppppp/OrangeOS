#include "stdio.h"
#include "string.h"

#define BUFFER_SIZE 256

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: cat <filename>\n");
        return 1;
    }

    // 打开文件
    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        printf("Failed to open %s\n", argv[1]);
        return 1;
    }

    char buf[BUFFER_SIZE];
    int n;

    //读取并输出 
    while ((n = read(fd, buf, BUFFER_SIZE)) > 0) {
        write(1, buf, n);  // 1 是标准输出文件描述符
    }

    // 关闭文件
    close(fd);
    return 0;
}
