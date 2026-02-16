#ifndef CONSOLE_RENDER_H
#define CONSOLE_RENDER_H

#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>

// 在 cg_gfx.cpp 内部
struct Pixel {
    char ch = ' ';
    const char* color = "\033[0m"; // 默认颜色（重置符）
};

class ConsoleRenderer {
private:
    int width, height;
    std::vector<Pixel> buffer; // 升级为 Pixel 向量

public:
    ConsoleRenderer(int w, int h) : width(w), height(h) {
        buffer.assign(width * height, {' ', "\033[0m"});
    }

    void resize(int w, int h) {
        width = w; height = h;
        buffer.assign(width * height, {' ', "\033[0m"});
    }

    void clear() {
        std::fill(buffer.begin(), buffer.end(), Pixel{' ', "\033[0m"});
    }
    void draw_char(int x, int y, char ch) {
        // 调用刚刚写好的新函数，颜色传入重置符
        draw_char_ex(x, y, ch, CG_COLOR_RESET);
    }
    void draw_char_ex(int x, int y, char ch, const char* color) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            int idx = y * width + x;
            buffer[idx].ch = ch;
            buffer[idx].color = color ? color : "\033[0m";
        }
    }

    void render() {
        std::string out = "\033[H\033[?25l";
        const char* lastColor = "\033[0m";

        for (int i = 0; i < height * width; ++i) {
            // 颜色优化：只有颜色变化时才插入 ANSI 码，大幅减少传输数据量
            if (buffer[i].color != lastColor) {
                out += buffer[i].color;
                lastColor = buffer[i].color;
            }
            out += buffer[i].ch;
            if ((i + 1) % width == 0) out += '\n';
        }
        out += "\033[0m"; // 结尾重置
        write(STDOUT_FILENO, out.c_str(), out.size());
    }
};
#endif //CONSOLE_RENDER_H