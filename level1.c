#include<termios.h>  
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<stdio.h>

struct termios orig_termios;//存储终端的当前设置

void disableRawMode(){
	printf("Exiting raw mode...\n");
	//raw.c_lflag &= (ECHO);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);//将原始设置输回终端
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);//获取当前终端设置并存储到orig_termios中
  struct termios change = orig_termios;//存储终端的原始设置
  printf("Entering raw mode...\n");
  tcgetattr(STDIN_FILENO, &change);//读取标准输入终端的属性存储到change中
  
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

int main() {
  char c;
  enableRawMode(); //启用原始模式
  atexit(disableRawMode);//在退出时执行这个disableRawMode函数
  while (1){
  	while (read(STDIN_FILENO, &c, 1) != 1);//读取到的字符数不为1
	if(c == '\x1b')//读到了ESC键，进入控制符号处理
	{
		char seq[3];
		if (read(STDIN_FILENO, &seq[1], 1) != 1)
			break;
		if (read(STDIN_FILENO, &seq[2], 1) != 1)
			break;
		if (seq[1] == '[') {
			switch (seq[2]) {
				case 'A':
					printf("\nUp arrow key pressed.\n");
					break;
				case 'B':
					printf("\nDown arrow key pressed.\n");
					break;
				case 'C':
					printf("\nRight arrow key pressed.\n");
					break;
				case 'D':
					printf("\nLeft arrow key pressed.\n");
					break;
			}
		}
	}
	
	else if(c == 'q'){
		break;//如果读取到的字符是'q'，则退出
	}

	else  //如果读取到的字符不是'q'，则打印读取的字符
	{
		printf("\nRead character: %c\n", c); //打印读取的字符
	}

  }
  return 0;
}