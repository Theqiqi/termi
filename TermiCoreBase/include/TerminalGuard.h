#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <fcntl.h>
#include <errno.h>

class TerminalGuard {
public:
    TerminalGuard() {
        // 1. 备份当前终端配置
        if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
            perror("tcgetattr");
            exit(1);
        }

        // 2. 配置 Raw Mode
        struct termios raw = orig_termios;

        // c_lflag (本地模式):
        // ECHO: 禁止回显字符
        // ICANON: 禁止标准模式（即：不需要回车就能读取）
        // ISIG: 禁用终端控制字符（如 Ctrl+C, Ctrl+Z，后期可以根据需要开启）
        raw.c_lflag &= ~(ECHO | ICANON | ISIG);

        // 3. 应用配置
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
            perror("tcsetattr");
            exit(1);
        }
        // 设置 STDIN 为非阻塞模式
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    }

    // 析构函数：确保程序结束时恢复终端
    ~TerminalGuard() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    }
    // 封装一个简单的读取函数
    int readKey() {
        unsigned char ch;
        ssize_t nread = read(STDIN_FILENO, &ch, 1);

        if (nread == -1) {
            // 在非阻塞模式下，如果没有数据可读，read 会返回 -1
            // 且 errno 会被设置为 EAGAIN
            if (errno == EAGAIN) return 0;
            return -1; // 真正的错误
        }

        if (nread == 0) return 0; // 没读到数据
        return ch;
    }
private:
    struct termios orig_termios;
};