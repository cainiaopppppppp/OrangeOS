#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"

PUBLIC char* search_dir(char* path) {
    MESSAGE msg;
    msg.type = SEARCH;
    memcpy(msg.Path, path, strlen(path));

    send_recv(BOTH, TASK_FS, &msg);
    return msg.Path;
}

