#include<termios.h>  
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
struct termios orig_termios; 
#define TAB_SIZE 4 // 定义制表符大小为4个空格

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

typedef struct erow{//行内容结构体
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

void editorRenderRow(erow *row) {
	// 渲染行内容
	int tabs = 0;
	for (int i = 0; i < row->size; i++) {
		if (row->chars[i] == '\t') {
			tabs++;
		}
	}
	// 计算渲染版本的大小
	row->rsize = row->size + tabs * (TAB_SIZE - 1);//替换，所以减一
	row->render = malloc(row->rsize + 1);
	int idx = 0;
	for (int i = 0; i < row->size; i++) {
		if (row->chars[i] == '\t') {
			for (int k = 0; k < TAB_SIZE; k++) {
				row->render[idx++] = ' ';
			}
		} else {
			row->render[idx++] = row->chars[i];
		}
	}
	row->render[row->rsize] = '\0';
}

void editorAppendRow(editorConfig *E, const char *s, size_t len) {
	E->row = realloc(E->row, sizeof(erow) * (E->numrows + 1));// 扩展行数组,增加一行
	erow *new_row = &E->row[E->numrows];//下一行起点地址
	E->numrows++; // 更新行数


	new_row->size = len;
	new_row->chars = malloc(len + 1);
	memcpy(new_row->chars, s, len);
	new_row->chars[len] = '\0';


	// 渲染版本的内容
	new_row->rsize = 0; // 初始化渲染版本大小
	new_row->render = NULL; // 初始化渲染版本内容为空
	editorRenderRow(new_row); // 渲染行内容
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

int rows, cols; // 全局变量，用于存储窗口大小

int main(int argc, char *argv[]){
	int get_window_size(int *rows, int *cols);
	editorConfig E = {0}; // 初始化编辑器配置
	E.cx = 0; // 光标初始位置
	E.cy = 0;
	E.viewrowoff = 0; // 视图行偏移
	E.viewcoloff = 0; // 视图列偏移	
	E.numrows = 0; // 初始化行数为0
	E.row = NULL; // 初始化行数组为空	

	if (argc == 1) {
		printf("hello world\n");
		return 0;
	}
	if (argc == 2)
	{
		editoropenfile(&E, argv[1]);
		enableRawMode();
		atexit(disable_raw_mode); // 在程序退出时恢复原始终端
	}
	return 0;
}