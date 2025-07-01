#include<termios.h>  
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<stdio.h>
#include<errno.h>

struct  termios orig_termios;

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

int readKey() {
	int nread ;
	char c;
	//持续读取知道一个字符被读取
	while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
		if (nread == -1 && errno != EAGAIN) {
			perror("read");
			return -1;
		}
	}

	if (c == '\x1b') { // 读取到的是ESC键
		char seq[4];
		// 读取接下来的两个字符
		if (read(STDIN_FILENO, &seq[1], 1) != 1)
			return -1;
		if (read(STDIN_FILENO, &seq[2], 1) != 1)
			return -1;
		if (read(STDIN_FILENO, &seq[3], 1) != 1)
			return -1;

		//标准转义序列“\x1b[”开头
		if (seq[1] == '[') {
			//设置颜色的转义序列以及擦除功能的转义序列,这里只实现清屏
			if(seq[2] >= '0' && seq[3] <= '9') {
				if(seq[3]=='J'&&seq[2]=='2') {
					
					return 1000; // 清屏的转义序列
				}
			}
			switch (seq[2]) {
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
				case 'F':
					//printf("\nEnd key pressed.\n");
					return 1006;
				default:
					printf("\nUnknown escape sequence: %s\n", seq);
			}
		}
		else {
			printf("\nUnknown escape sequence: %c%c%c\n", seq[0], seq[1], seq[2]);
		}
		return 0;
	}
	else {
		printf("Key pressed: %c\n", c);
		return c;
	}

}

void adAppend(){

}


int main() {
    enableRawMode();
	printf("level2.c程序已启动\n");
	write(STDOUT_FILENO,"\x1b[2J", 4);//清屏
	write(STDOUT_FILENO,"\x1b[H", 3);//光标移动到左上角
	
	while (1){
		int key = readKey();
		if (key == -1) {
			break; // 读取错误，退出循环
		}
		else if (key == 1000) {
			write(STDOUT_FILENO,"\x1b[2J", 4);//清屏
			write(STDIN_FILENO,"\x1b[H", 3);//光标移动到左上角
			printf("level2.c程序已启动\n");
			//printf("\nUp arrow key pressed.\n");
		}
		else if (key == 1001) {
			write(STDIN_FILENO,"\x1b[A", 3);//光标移动到上方
			//printf("\nUp arrow key pressed.\n");
		}
		else if (key == 1002) {
			write(STDIN_FILENO,"\x1b[B", 3);//光标移动到下方
			//printf("\nDown arrow key pressed.\n");
		}
		else if (key == 1003) {
			write(STDIN_FILENO,"\x1b[C", 3);//光标移动到右方
			//printf("\nRight arrow key pressed.\n");
		}
		else if (key == 1004) {
			write(STDIN_FILENO,"\x1b[D", 3);//光标移动到左方
			//printf("\nLeft arrow key pressed.\n");
		}
		else if (key == 1005) {
			write(STDIN_FILENO,"\x1b[H", 3);//光标移动到左上角
			//printf("\nHome key pressed.\n");
		}
		else if (key == 1006) {
			write(STDIN_FILENO,"\x1b[F", 3);//光标移动到右下角
			//printf("\nEnd key pressed.\n");
		}
		else if (key == 'q') {
			break; // 如果读取到的字符是'q'，则退出
		}
		else {
			//printf("\nRead character: %c\n", key); // 打印读取的字符
		}
	}

    return 0;
}