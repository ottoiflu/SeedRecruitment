#include<termios.h>  
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<stdio.h>
#include<errno.h>

int disableRawMode(struct  termios orig_termios){
	printf("Exiting raw mode...\n");
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
		perror("tcsetattr");
	}
	return 0;
}

void enableRawMode() {
	struct  termios orig_termios;
  if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) {//成功为0，失败为-1
      perror("tcgetattr");
  }
  struct termios change = orig_termios;//存储终端的原始设置
  printf("Entering raw mode...\n");
  if(tcgetattr(STDIN_FILENO, &change) == -1) {//读取标准输入终端的属性存储到change中
      perror("tcgetattr");
  }

  change.c_lflag &= ~(ECHO);//修改本地标志字段，关闭回显功能
  change.c_lflag &= ~(ICANON);//关闭规范模式，启用原始模式
  change.c_lflag &= ~(ISIG);//关闭信号处理

  change.c_iflag &= ~(BRKINT);//输入模式标志，关闭中断处理
  change.c_iflag &= ~(BRKINT|INPCK|ISTRIP);//检测到输入终止条件时发送SIGINT信号、奇偶校验、裁剪掉第8比特，将所有接收到的数据裁剪为 7 比特位
  change.c_iflag &= ~(IXON);//输出流控制

  change.c_cc[VMIN] = 0; //设置最小输入字符数为0
  change.c_cc[VTIME] = 1; //设置输入等待时间为1

  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &change) == -1) {//将修改后的传入标准输入终端
      perror("tcsetattr");
  }
  //tcsaflush在更改属性前刷新所有的输入输出

}