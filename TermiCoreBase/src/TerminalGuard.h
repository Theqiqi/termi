#pragma once

#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
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
    int readKey()
    {
        unsigned char ch;
        ssize_t nread = read(STDIN_FILENO, &ch, 1);
        if (nread <= 0) return 0;
        if (ch == 27) { // 发现 Esc
            unsigned char next[2];
            // 尝试以非阻塞方式再读两个字符
            if (read(STDIN_FILENO, &next[0], 1) <= 0) return 27; // 后面没东西，真的是 Esc
            if (read(STDIN_FILENO, &next[1], 1) <= 0) return 0;  // 只有部分序列，丢弃

            if (next[0] == '[') {
                switch (next[1]) {
                    case 'A': return 'w'; // 将方向键上映射为 'w'
                    case 'B': return 's'; // 方向键下
                    case 'C': return 'd'; // 方向键右
                    case 'D': return 'a'; // 方向键左
                }
            }
            return 0; // 其他复杂的转义序列暂时忽略
        }
        return ch;
    }

private:
    struct termios orig_termios;
};
