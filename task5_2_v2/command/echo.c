#include "stdio.h"
#include "string.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: echo <text> <filename>\n");
        return 1;
    }

    // 打开或创建文件以写入数据
    int fd = open(argv[argc - 1], O_RDWR | O_CREAT);
    if (fd < 0) {
        printf("Failed to open or create %s\n", argv[argc - 1]);
        return 1;
    }

	int i;
    // 遍历所有参数并写入文件，除了最后一个参数（文件名）
    for (i = 1; i < argc - 1; i++) {
        write(fd, argv[i], strlen(argv[i]));
        if (i < argc - 2) {
            write(fd, " ", 1);  // 在单词之间添加空格
        }
    }
    write(fd, "\n", 1);  // 添加换行符

    // 关闭文件
    close(fd);
    return 0;
}
