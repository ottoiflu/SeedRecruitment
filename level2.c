#include<termios.h>  
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>

#include"buffer.h"
#include"editor_core.h"

//用于光标定位与视图偏移
int cy = 0,cx = 0; // 光标x,y坐标
int rowoff = 0, coloff = 0; // 行偏移和列偏移

int get_window_size(int *rows, int *cols) {
	struct winsize ws;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
	*rows = ws.ws_row;
	*cols = ws.ws_col;
	return 0; // 成功获取窗口大小，返回0
}

int cols, rows;
int readKey() {
	int nread ;
	char c;
	//持续读取知道一个字符被读取
	nread = read(STDIN_FILENO, &c, 1);
	if(nread != 1)return -1; // 如果读取不到一个字符，返回-1

	if (c == '\x1b') { // 读取到的是ESC键
		char seq[2];
		// 读取接下来的两个字符
		if (read(STDIN_FILENO, &seq[0], 1) != 1)
			return '\x1b';
		if (read(STDIN_FILENO, &seq[1], 1) != 1)
			return '\x1b';

		//标准转义序列“\x1b[”开头
		if (seq[0] == '[') {
			switch (seq[1]) {
				case 'A':
					//printf("\nUp arrow key pressed.\n");
					return 1001;
				case 'B':
					//printf("\nDown arrow key pressed.\n");
					return 1002;
				case 'C':
					//printf("\nRight arrow key pressed.\n");
					return 1003;
				case 'D':
					//printf("\nLeft arrow key pressed.\n");
					return 1004;
				case 'H':
					//printf("\nHome key pressed.\n");
					return 1005;
				case 'G':
					//printf("\nEnd key pressed.\n");
					return 1006;
			}
		}
		return 0;
	}
		return c;
	}

void scroll() {
	if (cy < rowoff) {
		rowoff = cy; // 保持光标在第一行
	}
	if (cy >= rowoff+rows) {
		rowoff= cy - rows+1; // 向上滚动
	}

	if (cx < coloff) {
		coloff = cx; // 保持光标在第一列
	}
	if (cx >= coloff+cols) {
		coloff = cx - cols + 1; // 向左滚动
	}
}

void refresh_screen() {
	scroll(); // 调用滚动函数
	struct appendbuffer ab = APPENDBUFFER_INIT;
	bufferappend(&ab, "\x1b[H", 3); // 光标移动到左上角
	bufferappend(&ab, "\x1b[3J", 4); //

	for (int i = 0; i < rows; i++) {
		int filerow = i + rowoff; // 计算实际行号
		char line[64];
		int linelen = snprintf(line, sizeof(line), "~ line %d\r\n", filerow);
		bufferappend(&ab, line, linelen);
	}

	char buf[32];
	sprintf(buf, "\x1b[%d;%dH", cy -rowoff+1, cx -coloff+1); // 光标位置
	bufferappend(&ab, buf, strlen(buf)); // 添加光标位置到缓冲区

	write(STDOUT_FILENO, ab.b, ab.len); // 输出缓冲区内容
	free(ab.b); // 释放缓冲区

}



int main() {
	struct termios orig_termios;//存储终端的标准设置
	if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
		perror("tcgetattr");
	}
    enableRawMode();
	


	struct appendbuffer ab = APPENDBUFFER_INIT;
	bufferappend(&ab, "\x1b[H", 3); // 光标移动到左上角
	bufferappend(&ab, "\x1b[2J", 4); // 清屏
	bufferappend(&ab, "\x1b[3J", 4); //
	bufferappend(&ab, "level2.c start\n", 16); // 添加文本到缓冲区
	bufferappend(&ab, "\x1b[H", 3); // 光标移动到左上角
	write(STDOUT_FILENO, ab.b, ab.len); // 输出缓冲区内容
	free(ab.b);


	while (1){
		get_window_size(&rows, &cols); // 获取窗口大小
		refresh_screen(); // 刷新屏幕
		int key = readKey();
		struct appendbuffer ab = APPENDBUFFER_INIT;
		
		
		if (key == 1000) {
			
			bufferappend(&ab, "\x1b[2J", 4); // 清屏
			bufferappend(&ab, "\x1b[H", 3); // 光标移动到左上角
			bufferappend(&ab, "level2.c程序已启动\n", 16); // 添加文本到缓冲区
			
			
		}
		else if (key == 1001) {
			if(cy > 0) {
				cy--; // 光标移动到上方
			} else {
				cy = 0; // 如果已经在第一行，则不移动
			}
			
		}
		else if (key == 1002) {
				cy++; // 光标移动到下方
				
		}
		else if (key == 1003) {
				cx++; // 光标移动到右方
				
		}
		else if (key == 1004) {
			if(cx > 0) {
				cx--; // 光标移动到左方
			} else {
				cx = 0; // 如果已经在第一列，则不移动
			}
		}
		else if (key == 1005) {
			cx = 0; // 光标移动到行首
			cy = 0; // 光标移动到第一行
		}
		else if (key == 1006) {
			cx = cols - 1; // 光标移动到行尾
		}
		else if (key == 'q') {
			bufferappend(&ab, "\x1b[2J", 4); // 清屏
			disableRawMode(orig_termios); // 禁用原始模式
			break; // 如果读取到的字符是'q'，则退出
		}
		else {
			//printf("\nRead character: %c\n", key); // 打印读取的字符
		}
		write(STDOUT_FILENO, ab.b, ab.len); // 输出缓冲区
		free(ab.b); // 释放缓冲区
	}
	
    return 0;
}