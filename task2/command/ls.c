// /command/ls.c
#include "stdio.h"
#include "const.h"
#include "string.h"
#include "fs.h"

int main (int args, char* argv[])
{
    char* result;
    result = search_dir("/");
    printf("%s\n", result);
    return 0;
}