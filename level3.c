#include<termios.h>  
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
struct termios orig_termios; 

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);//获取当前终端设置并存储到orig_termios中
  struct termios change = orig_termios;//存储终端的原始设置
  printf("Entering raw mode...\n");
  
  change.c_lflag &= ~(ECHO);//修改本地标志字段，关闭回显功能
  change.c_lflag &= ~(ICANON);//关闭规范模式，启用原始模式
  change.c_lflag &= ~(ISIG);//关闭信号处理

  change.c_iflag &= ~(BRKINT);//输入模式标志，关闭中断处理
  change.c_iflag &= ~(BRKINT|INPCK|ISTRIP);//检测到输入终止条件时发送SIGINT信号、奇偶校验、裁剪掉第8比特，将所有接收到的数据裁剪为 7 比特位
  change.c_iflag &= ~(IXON);//输出流控制

  change.c_cc[VMIN] = 0; //设置最小输入字符数为0
  change.c_cc[VTIME] = 1; //设置输入等待时间为1

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &change);//将修改后的传入标准输入终端
  //tcsaflush在更改属性前刷新所有的输入输出

}


typedef struct erow{
	int size ;
	char *chars; //行内容
	int rsize; //行内容的渲染版本的大小
	char *render; //行内容的渲染版本
}erow;

typedef struct editorConfig {
	int cx, cy; //光标位置
	int screenrows, screencols; //屏幕行数和列数
	int viewrowoff, viewcoloff; //视图偏移
	int numrows; //行数
	erow *row; //行数组
}editorConfig;

void editorAppendRow(editorConfig *E, const char *s, size_t len) {
	E->row = realloc(E->row, sizeof(erow) * (E->numrows + 1));// 扩展行数组
	erow *new_row = &E->row[E->numrows];
	new_row->size = len;
	new_row->chars = malloc(len + 1);
	memcpy(new_row->chars, s, len);
	new_row->chars[len] = '\0';
	new_row->rsize = len; // 假设渲染版本与原始内容相同
	new_row->render = malloc(len + 1);
	memcpy(new_row->render, s, len);
	new_row->render[len] = '\0';
	E->numrows++;
}

void editoropenfile(editorConfig *E, const char *filename) {
	FILE *fp = fopen(filename, "r");
	if (!fp) {
		perror("fopen");
		return;
	}
	char *line = NULL;
	size_t len = 0;
	while (getline(&line, &len, fp) != -1) {
		editorAppendRow(E, line, strlen(line));
	}
	free(line);
	fclose(fp);
}


int main(int argc, char *argv[]){
	if (argc >= 2) {
		editoropenfile(&E, argv[1]);
		enableRawMode();
		atexit(disable_raw_mode); // 在程序退出时恢复原始终端
		return 1;
	}
}