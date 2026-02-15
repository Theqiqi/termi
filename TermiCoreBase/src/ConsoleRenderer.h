#include <vector>
#include <string>
#include <iostream>

class ConsoleRenderer {
public:
    ConsoleRenderer(int w, int h) : width(w), height(h) {
        // 初始化缓冲区，填充空格
        buffer.assign(width * height, ' ');
    }

    // 调整缓冲区大小（对应 T1.3 的窗口变化）
    void resize(int w, int h) {
        width = w;
        height = h;
        buffer.assign(width * height, ' ');
    }

    // 在指定坐标设置字符
    void draw_char(int x, int y, char ch) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            buffer[y * width + x] = ch;
        }
    }

    // 清空缓冲区
    void clear() {
        std::fill(buffer.begin(), buffer.end(), ' ');
    }

    // 将缓冲区推送到屏幕
    void render() {
        // 1. 光标复位到左上角 (H) 并隐藏光标 (?25l)
        std::string out = "\033[H\033[?25l";

        // 2. 逐行拼接缓冲区内容
        for (int i = 0; i < height; ++i) {
            out.append(&buffer[i * width], width);
            if (i < height - 1) out += '\n';
        }

        // 3. 一次性写入系统标准输出
        write(STDOUT_FILENO, out.c_str(), out.size());
    }

    ~ConsoleRenderer() {
        // 程序结束时显示光标
        std::cout << "\033[?25h" << std::flush;
    }

private:
    int width, height;
    std::vector<char> buffer;
};