#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ConsoleCanvas ConsoleCanvas;

// 初始化与销毁
ConsoleCanvas* cg_init();
void cg_close(ConsoleCanvas* canvas);

// 输入
int cg_read_key(ConsoleCanvas* canvas);

// 绘制
void cg_clear(ConsoleCanvas* canvas);
void cg_draw_pixel(ConsoleCanvas* canvas, int x, int y, char ch);
void cg_present(ConsoleCanvas* canvas);

// 尺寸
int cg_width(const ConsoleCanvas* canvas);
int cg_height(const ConsoleCanvas* canvas);

int cg_get_key(ConsoleCanvas* canvas);
// 检查窗口变化（返回 1 表示变化，0 表示无变化）
int cg_check_resize(ConsoleCanvas* canvas);
// include/cg_gfx.h
void cg_resize(ConsoleCanvas* canvas, int width, int height);
// 绘制一条直线
void cg_draw_line(ConsoleCanvas* canvas, int x1, int y1, int x2, int y2, char ch);

// 绘制一个矩形（空心或实心）
void cg_draw_rect(ConsoleCanvas* canvas, int x, int y, int w, int h, char ch, bool fill);

// 绘制一个圆形
void cg_draw_circle(ConsoleCanvas* canvas, int centerX, int centerY, int radius, char ch);
// 增加颜色常量定义
#define CG_COLOR_RESET  "\033[0m"

#define CG_COLOR_BLACK  "\033[30m"
#define CG_COLOR_RED    "\033[31m"
#define CG_COLOR_GREEN  "\033[32m"
#define CG_COLOR_YELLOW "\033[33m"
#define CG_COLOR_BLUE   "\033[34m"
#define CG_COLOR_MAGENTA "\033[35m"
#define CG_COLOR_CYAN   "\033[36m"
#define CG_COLOR_WHITE  "\033[37m"

#define CG_COLOR_GRAY   "\033[90m"

// 在绘图接口中增加颜色参数（或者创建一个新的 Ex 接口）
void cg_draw_pixel_ex(ConsoleCanvas* canvas, int x, int y, char ch, const char* color);

#ifdef __cplusplus
}
#endif