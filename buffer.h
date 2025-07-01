#include<termios.h>  
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>

struct appendbuffer{
	char *b;
	int len;
};

#define APPENDBUFFER_INIT {NULL, 0}


void bufferappend(struct appendbuffer *ab, const char *s, int len) {
	char* new_buffer = (char*)realloc(ab->b, ab->len + len);
	if (!new_buffer) {
		perror("realloc");
		exit(1);
	}
	ab->b = new_buffer;
	memcpy(ab->b + ab->len, s, len);//将新数据复制到缓冲区的末尾
	ab->len += len;
}