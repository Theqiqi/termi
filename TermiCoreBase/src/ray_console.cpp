#include "../include/ray_console.h"

#include <cstring>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <cstdlib>
#include <csignal>
// 内部全局状态
static ConsoleCanvas* _context = nullptr;
static bool _should_close = false;
static int _current_key_frame = 0;

void InitConsole(int width, int height) {
    // 1. 优先注册信号，防止初始化一半时被 Ctrl+C 搞崩
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);

    if (!_context) {
        // 2. 率先切换到备用缓冲区 (这就是进入“全屏游戏模式”)
        // \033[?1049h: 备用屏, \033[H: 光标归位, \033[2J: 清屏, \033[?25l: 隐藏光标
        printf("\033[?1049h\033[H\033[2J\033[?25l");
        fflush(stdout);

        // 3. 此时再初始化 context，它拿到的就是备用屏幕的纯净环境
        _context = cg_init();

        if (width > 0 && height > 0) {
            cg_resize(_context, width, height);
        }
    }
}

// ray_console.cpp 内部
void BeginDrawing() {
    cg_clear(_context);
    // 每一帧开始时，只读取一次按键存起来
    _current_key_frame = cg_get_key(_context);
}
static int _target_ms = 16; // 默认 60fps

int GetScreenWidth(void) { return cg_width(_context); }
int GetScreenHeight(void) { return cg_height(_context); }

void SetTargetFPS(int fps) {
    _target_ms = 1000 / fps;
}

void EndDrawing() {
    cg_present(_context);
    // 统一在这里控制帧率
    std::this_thread::sleep_for(std::chrono::milliseconds(_target_ms));
}

bool ConsoleWindowShouldClose() {
    // 这里可以处理 ESC 键或者退出信号
    return _should_close;
}

// 基于底层 API 封装高层函数
void DrawCircle(int cx, int cy, int r, char ch) {
    // 简单的视觉修正：水平半径乘以 2
    // 或者在底层算法中，对 x 坐标进行 2.0 的比例缩放
    cg_draw_line(_context, cx, cy, r * 2, r, ch);
}

// --- 1. 生命周期补全 ---

void CloseConsole(void) {
    if (_context) {
        cg_close(_context);
        _context = nullptr;
        printf("\033[?1049l\033[?25h");
    }
}

// --- 2. 状态控制补全 ---

void ClearBackground(void) {
    if (_context) cg_clear(_context);
}

// --- 3. 高级绘图补全 ---

// 矩形绘制：基于循环或底层的 line 函数
void DrawRectangle(int x, int y, int width, int height,char ch) {
    if (!_context) return;
    for (int i = 0; i < height; ++i) {
        // 画横线：利用之前实现的 cg_draw_line 或直接写像素
        cg_draw_line(_context, x, y + i, x + width - 1, y + i, ch);
    }
}

// 文本渲染：这在控制台中非常实用
void DrawText(int x, int y,const char* text) {
    if (!_context || !text) return;
    int len = strlen(text);
    for (int i = 0; i < len; ++i) {
        DrawPixelEx( x + i, y,text[i],CG_COLOR_RESET);
    }
}

// --- 4. 输入检测补全 ---
bool IsKeyPressed(int key) {
    return _current_key_frame == key;
}

// --- 5. 像素与直线（简单的映射调用） ---
// 兼容旧函数：让旧函数调用新接口，传入默认颜色
void DrawPixel( int x, int y,char ch) {
    DrawPixelEx(x, y, ch, CG_COLOR_RESET);
}
void DrawLine(int x1, int y1, int x2, int y2, char ch) {
    cg_draw_line(_context, x1, y1, x2, y2, ch);
}

void DrawPixelEx(int x, int y, char ch,const char* color) {
    if (_context) cg_draw_pixel_ex(_context, x, y, ch, color);
}

void DrawTextEx(int x, int y, const char* text,const char* color) {
    for (int i = 0; text[i] != '\0'; i++) {
        DrawPixelEx(x + i, y, text[i], color);
    }
}

int GetKeyPressed(void) {
    return _current_key_frame;
}

void PlayConsoleSound(void) {
    // 打印控制字符 \a，不占位，但会触发声音
    printf("\a");
    fflush(stdout);
}

void SignalHandler(int sig) {
    (void)sig;
    CloseConsole(); // 强行触发清理
    exit(0);
}
