#include "stdio.h"

int main(int args, char * argv[])
{
    if(~open(argv[1], O_CREAT)) {
        printf("Successfully created %s.\n", argv[1]);
    }else {
        printf("Failed to create %s, maybe because this file has been existed.\n",argv[1]);
    }
    return 0;
}