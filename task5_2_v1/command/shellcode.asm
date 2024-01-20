[section .text]

global _start

_start:
    pushad
    jmp get_msg_addr

main:
    pop edx
    xor eax, eax
    int 0x90
    popad
    mov ebx, 0x2181152    ; 随意编写的入口地址，在inject时会用原本的入口地址覆盖掉
    jmp ebx

get_msg_addr:
    call main

msg:
    db "Surpise! DP has injected it!", 0xA, 0x0
