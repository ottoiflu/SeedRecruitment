#include<termios.h>  
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>

#include"buffer.h"





void refresh_screen(int cx,int cy) {
	struct appendbuffer ab = APPENDBUFFER_INIT;
	// 光标移动到左上角
	bufferappend(&ab, "\x1b[H", 3);
	// 清屏
	bufferappend(&ab, "\x1b[2J", 4);
	// 设置光标位置
	char buf[32];
	int len = snprintf(buf, sizeof(buf), "\x1b[%d;%dH", cy + 1, cx + 1);
	bufferappend(&ab, buf, len);
	// 输出缓冲区内容
	write(STDOUT_FILENO, ab.b, ab.len);
	free(ab.b);
}

int get_window_size(int *rows, int *cols) {
	struct winsize ws;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
	*rows = ws.ws_row;
	*cols = ws.ws_col;
	return 0; // 成功获取窗口大小，返回0
}

void scoll(int *cx, int *cy, int *rows, int *cols, int *rowoff, int *coloff) {
	// 处理滚动逻辑
	// 这里可以添加滚动的实现
	// 例如，当光标到达屏幕底部时，向上滚动一行
	if (*cy < *rowoff) {
		*rowoff = *cy; // 保持光标在最后一行
	}
	if (*cy >= *rowoff + *rows) {
		*rowoff = *cy - *rows + 1; // 向上滚动
	}
	if (*cx < *coloff) {
		*coloff = *cx; // 保持光标在最后一列
	}
	if (*cx >= *coloff + *cols) {
		*coloff = *cx - *cols + 1; // 向左滚动
	}
}