#include "stdio.h"

int main(int args, char* argv[]) {
    if (args != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    } else {
        if (unlink(argv[1]) == -1) {
            printf("Failed to remove %s\n", argv[1]);
            return 1;
        }
        printf("%s is successfully removed\n", argv[1]);
    }
    return 0;
}
