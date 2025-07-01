# 题目思路

## level 0	
运行方法：
- make level1
- make level2

## level 1

- 核心是disableRawMode()与enableRawMode()函数的配合使用，确保在进入和退出原始模式时正确保存和恢复终端设置。
- 封装进editor_core.h中，方便后续调用


## level 2

- 缓存方法
  - 使用appendbuffer结构体来管理缓冲区，包含一个指向字符数组的指针和当前长度。
	```c
	struct appendbuffer{
		char *b;
		int len;
		};
	```
  - 使用`bufferappend`函数来追加字符串到缓冲区中，使用`realloc`来动态调整缓冲区大小。
	```c
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
	```
  - 一次性输出缓存，并释放内存
	```c
	write(STDOUT_FILENO, ab.b, ab.len); // 输出缓冲区
	free(ab.b); // 释放缓冲区
	```
- 显示模块
  - 获取终端窗口大小
	```c
	int get_window_size(int *rows, int *cols) {
		struct winsize ws;
		ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
		*rows = ws.ws_row;
		*cols = ws.ws_col;
		return 0; // 成功获取窗口大小，返回0
	}
	```
  - 光标定位与视图偏移
	```c
	int cy = 0,cx = 0; // 光标x,y坐标
	int rowoff = 0, coloff = 0; // 行偏移和列偏移
	```


## level 3
>由于level 2 中显示模块存在严重bug，level 3只实现了读取文件的功能
- 结构体定义
	
  - 行结构体
  	```c
  	typedef struct erow{//行内容结构体
  		int size ;//行内容的大小
  		char *chars; //行内容
  		int rsize; //行内容的渲染版本的大小
  		char *render; //行内容的渲染版本
  	}erow;
  	```
  - 编辑器结构体
  	```c
  	typedef struct editorConfig{
  		int cx, cy; // 光标位置
  		int rowoff, coloff; // 行偏移和列偏移
  		int numrows; // 行数
  		erow *row; // 行内容数组
  	}editorConfig;
  	```


- 打开文件、读入行内容数组、渲染制表符
  - 打开文件
	```c
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
	```
    - 追加行内容到行内容数组
  >在上面的while循环中，使用`getline`函数读取每一行，并调用`editorAppendRow`函数将行内容追加到编辑器配置结构体的行数组中。

		```c
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
		```

	- 渲染制表符
	>在`editorRenderRow`函数中，遍历行内容，将制表符替换为空格，并计算渲染版本的大小和内容。
	```c
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
	````