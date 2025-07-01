#include<termios.h>  
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<stdio.h>
#include<errno.h>
#include"editor_core.h"

int main() {
  char c;
  struct termios orig_termios;//存储终端的标准设置
  if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
      perror("tcgetattr");
  }

  enableRawMode(); //启用原始模式
  //atexit(disableRawMode);//在退出时执行这个disableRawMode函数
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
					printf("\nKey:PageUp \n");
					break;
				case 'B':
					printf("\nKey:PageDown \n");
					break;
				case 'C':
					printf("\nKey:RightArrow \n");
					break;
				case 'D':
					printf("\nKey:LeftArrow \n");
					break;
			}
		}
	}
	
	else if(c == 'q'){
		disableRawMode(orig_termios); //禁用原始模式
		break;//如果读取到的字符是'q'，则退出
	}

	else  //如果读取到的字符不是'q'，则打印读取的字符
	{
		printf("\nKey: %c   ASCII:%d\n", c, c); //打印读取的字符
	}

  }
  return 0;
}