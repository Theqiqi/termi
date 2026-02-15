#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

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
    }

    // 析构函数：确保程序结束时恢复终端
    ~TerminalGuard() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    }

private:
    struct termios orig_termios;
};

int main() {
    TerminalGuard guard; // 开启保护伞

    std::cout << "终端已进入 Raw Mode。按任意键测试（Ctrl+C 已被禁用，需等待 3 秒自动退出）..." << std::endl;

    // 简单睡眠 3 秒，期间你可以乱敲键盘
    sleep(3);

    return 0;
}