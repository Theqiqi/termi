#include "cg_gfx.h"
#include <memory>
#include "ConsoleRenderer.h"
#include "TerminalGuard.h"
#include "TerminalTopology.h"

// 内部隐藏结构体
struct ConsoleCanvas {
    TerminalGuard guard;
    TerminalTopology topology;
    std::unique_ptr<ConsoleRenderer> renderer;

    ConsoleCanvas() {
        renderer = std::make_unique<ConsoleRenderer>(
            topology.get_width(), topology.get_height()
        );
    }
};

// --- C 风格接口实现 ---

extern "C" ConsoleCanvas* cg_init() {
    try {
        printf("\033[?1049h\033[?25l\033[2J\033[H");
        fflush(stdout);
        return new ConsoleCanvas();
    } catch (...) {
        return nullptr;
    }
}

extern "C" void cg_close(ConsoleCanvas* canvas) {
    if (canvas) delete canvas;
    printf("\033[?25h\033[?1049l");
    fflush(stdout);
}

extern "C" void cg_clear(ConsoleCanvas* canvas) {
    if (canvas) canvas->renderer->clear();
}

extern "C" void cg_draw_pixel(ConsoleCanvas* canvas, int x, int y, char ch) {
    if (x < 0 || x >= cg_width(canvas) || y < 0 || y >= cg_height(canvas)) return;
    canvas->renderer->draw_char(x, y, ch);
}

extern "C" void cg_present(ConsoleCanvas* canvas) {
    if (!canvas) return;

    // 解决闪烁问题的关键：在渲染前最后时刻检查拓扑
    if (canvas->topology.check_and_update()) {
        int new_w = canvas->topology.get_width();
        int new_h = canvas->topology.get_height();
        canvas->renderer->resize(new_w, new_h);

        // 遇到剧烈缩放时，强制清屏一次以清除旧残影
        std::cout << "\033[2J" << std::flush;
    }

    canvas->renderer->render();
}

extern "C" int cg_width(const ConsoleCanvas* canvas) {
    return canvas ? canvas->topology.get_width() : 0;
}

extern "C" int cg_height(const ConsoleCanvas* canvas) {
    return canvas ? canvas->topology.get_height() : 0;
}

extern "C" int cg_get_key(ConsoleCanvas* canvas) {
    if (!canvas) return 0;
    return canvas->guard.readKey();
}


extern "C" int cg_check_resize(ConsoleCanvas* canvas) {
    if (!canvas) return 0;
    return canvas->topology.check_and_update() ? 1 : 0;
}
extern "C" void cg_resize(ConsoleCanvas* canvas, int width, int height) {
    if (canvas && canvas->renderer) {
        canvas->renderer->resize(width, height);
    }
}
// src/cg_gfx.cpp 实现层
//核心算法实现：Bresenham 直线算法
extern "C" void cg_draw_line(ConsoleCanvas* canvas, int x1, int y1, int x2, int y2, char ch) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;

    while (true) {
        cg_draw_pixel(canvas, x1, y1, ch);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}
//核心算法实现：中点圆算法
extern "C" void cg_draw_circle(ConsoleCanvas* canvas, int xc, int yc, int r, char ch) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    auto draw_8_points = [&](int x, int y) {
        cg_draw_pixel(canvas, xc + x, yc + y, ch); cg_draw_pixel(canvas, xc - x, yc + y, ch);
        cg_draw_pixel(canvas, xc + x, yc - y, ch); cg_draw_pixel(canvas, xc - x, yc - y, ch);
        cg_draw_pixel(canvas, xc + y, yc + x, ch); cg_draw_pixel(canvas, xc - y, yc + x, ch);
        cg_draw_pixel(canvas, xc + y, yc - x, ch); cg_draw_pixel(canvas, xc - y, yc - x, ch);
    };

    while (y >= x) {
        draw_8_points(x, y);
        x++;
        if (d > 0) { y--; d = d + 4 * (x - y) + 10; }
        else d = d + 4 * x + 6;
    }
}
// src/cg_gfx.cpp
extern "C" void cg_draw_pixel_ex(ConsoleCanvas* canvas, int x, int y, char ch, const char* color) {
    if (canvas && canvas->renderer) {
        canvas->renderer->draw_char_ex(x, y, ch, color);
    }
}

