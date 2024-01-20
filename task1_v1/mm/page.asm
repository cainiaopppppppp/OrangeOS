; %include	"lib.inc"	; 库函数
; %include	"pm.inc"	; 常量, 宏, 以及一些说明
[SECTION .data]
AvaLinearAddress       dd  0x8000_0000
BitMap:    times   32  db  0xff
            times   32  db  0x00
BitMapLen equ $ - $$

dwDispPos:			dd	(80 * 13 + 0) * 2

PageTableNumber		dd	0
szBASE				db	"Linear_address:", 0
szPDE				db  "PDE_base_address:", 0
szPTE				db  "PTE_base_address:", 0
szFINAL				db  "Physical_address:", 0
szAddressNotPresent db "Error: Virtual address not present", 0
VirtualAddr		equ	00401000h
szReturn			db	0Ah, 0
PG_P		EQU	1	; 页存在属性位
PG_RWR		EQU	0	; R/W 属性位值, 读/执行
PG_RWW		EQU	2	; R/W 属性位值, 读/写/执行
PG_USS		EQU	0	; U/S 属性位值, 系统级
PG_USU		EQU	4	; U/S 属性位值, 用户级

global DispTransfer
global	DispStr
global	DispInt
global	DispReturn
global	DispAL
global alloc_pages
global free_pages

DispTransfer:
	; 保存现有的寄存器状态
	xchg bx,bx
    pusha

	; 打印base_address
	call DispReturn
	push szBASE
	call DispStr
	add esp, 4

	; 打印虚拟地址
	mov eax, VirtualAddr
	push eax
	call DispInt
	add esp, 4

	; 打印PDE_base_address
	call DispReturn
	push szPDE
	call DispStr
	add esp, 4

	; 计算并打印页目录项
	; mov ax, SelectorFlatRW
	; mov es, ax
	mov eax, VirtualAddr
	shr eax, 22
	cmp eax, [PageTableNumber]
	; jg .error
	mov ebx, 4
	mul ebx
	mov edx, cr3
	add eax, edx
	mov ecx, dword [es:eax]
	xor ecx, PG_P | PG_USU |PG_RWW
	and ecx, 0xfffffc00
	push dword [es:eax]
	call DispInt
	add esp, 4

	; 打印PDE_base_address
	call DispReturn
	push szPTE
	call DispStr
	add esp, 4

	; 计算打印页表项
	mov eax, VirtualAddr
    shr eax, 12
    and eax, 0x3FF
    mov ebx, 4
    mul ebx
    add eax, ecx
    mov ecx, dword [es:eax]
    xor ecx, PG_P | PG_USU | PG_RWW
    and ecx, 0xfffff000
    push dword [es:eax]
    call DispInt
    add esp, 4

	; 打印Final_address
	call DispReturn
	push szFINAL
	call DispStr
	add esp, 4

	;计算打印最终地址
	mov eax, VirtualAddr
    and eax, 0xFFF
    add eax, ecx
    push eax
    call DispInt
    add esp, 4

    ; 恢复寄存器并返回
    popa
    ret

.error:
    ; 打印错误消息
    push szAddressNotPresent
    call DispStr
    add esp, 4

    ; 恢复寄存器并返回
    popa
    ret



free_pages:
	; push ds
	; push es
	push ebx         ; save eax and ebx
	push eax

	; mov bx, SelectorFlatRW	
	; mov ds, bx
	; mov bx, SelectorData
	; mov es, bx       ; normal init
		
	; find the pde and pte 
	mov ebx, cr3
	and ebx, 0xfffff000
	and eax, 0xffc00000	
	shr eax, 20      ; 20 = 22 -2 
	add ebx, eax
	mov edx, [ebx]
	and edx, 0xfffffff8
	mov [ebx], edx
	
	mov ebx, [ebx]
			
	mov eax, [esp]
	add esp, 4 
	and ebx,0xfffff000
	and eax,0x003ff000
	shr eax,10
	add ebx, eax


	mov ecx, [esp]          
	add esp,4
.free_pte:
	mov eax, [ebx]
	and eax, 0xfffffff8	
    mov edx, eax			    ; eax:映射的物理地址
	shr edx, 12               ;edx:第edx个页面（由于物理地址开始为0x00000000）
	btr [BitMap], edx         ;改变位图，设置为0，空闲
	mov [ebx], eax
	add ebx,32
	loop .free_pte

	; pop es
	; pop ds
	ret 

alloc_pages:

    ;save register
    ; push ds
    ; push es
    ; ;function implement
    ; mov bx, SelectorFlatRW 
    ; mov ds,bx

    ; mov bx,SelectorData
    ; mov es,bx

    mov ecx,eax

    ;计算总共需要分配的物理内存大小，并将其存储在 ebx 寄存器中。
    mov ebx,4096    ;page number * page size
    mul ebx

    mov ebx,[es:AvaLinearAddress]
    add [es:AvaLinearAddress],eax
    ;ebx存的是AvaLinearAddress 指定的线性地址对应的地址
    push ebx
    mov eax,ebx
    mov ebx,cr3
    and ebx,0xfffff000
    and eax,0xffc00000
    shr eax,20
    add ebx,eax     ;ebx中是PDE中的地址
    mov edx,ebx     ;edx->missing PDE address页目录项（PDE）的地址
    mov ebx,[ebx]   ;PTE 地址 
    ;test p flag 检查 PDE 中的 P 标志位
    test ebx,0x0000_0001
    jnz .pde_exist
    ;page table is not exist
    mov ebx,cr3
    mov ebx,[ebx]   ;first page table address
    and ebx,0xfffff000
    shl eax,10
    add ebx,eax     ;这里会直接变为页表的位置
    or ebx,0x0000_0007
    mov [edx],ebx

.pde_exist:
    mov eax,[esp]

    and ebx,0xfffff000
    and eax,0x003ff000
    shr eax,10
    add ebx,eax

.change_pte:
    call alloc_a_4k_page
    test eax,eax
    jz .allocation_failed   ;分配失败

    or eax,0x0000_0007
    mov [ebx],eax
    add ebx,4
    loop .change_pte
.allocation_failed:
    pop ebx
    ; pop es
    ; pop ds
    ret


alloc_a_4k_page:
                        ;return eax: physical address
    ;保存寄存器的值
    push ds
    push es
    ;function implement，设置合适的段寄存器的值，以确保访问正确的内存段
    xor eax,eax
    ; mov ax,SelectorFlatRW
    ; mov es,ax
    ; mov ax,SelectorData
    ; mov ds,ax

.search:
    bts [BitMap],eax

    jnc .find
    inc eax
    cmp eax,BitMapLen*8
    jl .search

    ;no available physical space，执行 hlt 指令来暂停 CPU 的执行，表示没有可用的物理空间。
    ;hlt;在分页分配中，使用 hlt 来表示分配失败并暂停 CPU 的执行是一种不常见的方式。
    mov eax,0
    ret

.find:

    shl eax,12

    pop es
    pop ds
    ret

DispStr:
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi

	mov	esi, [ebp + 8]	; pszInfo
	mov	edi, [dwDispPos]
	mov	ah, 0Fh
.1_str:
	lodsb
	test	al, al
	jz	.2_str
	cmp	al, 0Ah	; 是回车吗?
	jnz	.3_str
	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.1_str
.3_str:
	mov	[gs:edi], ax
	add	edi, 2
	jmp	.1_str

.2_str:
	mov	[dwDispPos], edi

	pop	edi
	pop	esi
	pop	ebx
	pop	ebp
	ret

DispInt:
	mov	eax, [esp + 4]
	shr	eax, 24
	call	DispAL

	mov	eax, [esp + 4]
	shr	eax, 16
	call	DispAL

	mov	eax, [esp + 4]
	shr	eax, 8
	call	DispAL

	mov	eax, [esp + 4]
	call	DispAL

	mov	ah, 07h			; 0000b: 黑底    0111b: 灰字
	mov	al, 'h'
	push	edi
	mov	edi, [dwDispPos]
	mov	[gs:edi], ax
	add	edi, 4
	mov	[dwDispPos], edi
	pop	edi

	ret

DispReturn:
	push	szReturn
	call	DispStr			;printf("\n");
	add	esp, 4

	ret

DispAL:
	push	ecx
	push	edx
	push	edi

	mov	edi, [dwDispPos]

	mov	ah, 0Fh			; 0000b: 黑底    1111b: 白字
	mov	dl, al
	shr	al, 4
	mov	ecx, 2
.begin:
	and	al, 01111b
	cmp	al, 9
	ja	.1_al
	add	al, '0'
	jmp	.2_al
.1_al:
	sub	al, 0Ah
	add	al, 'A'
.2_al:
	mov	[gs:edi], ax
	add	edi, 2

	mov	al, dl
	loop	.begin
	;add	edi, 2

	mov	[dwDispPos], edi

	pop	edi
	pop	edx
	pop	ecx

	ret