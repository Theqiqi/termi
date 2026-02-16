// include/ray_console.h
#ifndef RAY_CONSOLE_H
#define RAY_CONSOLE_H

#include <stdbool.h>

// 初始化与生命周期
void InitConsole(int width, int height);
bool ConsoleWindowShouldClose(void);
void CloseConsole(void);

// 绘图流程控制
void BeginDrawing(void);
void EndDrawing(void);
void ClearBackground(void);

// 高级绘图 API (基于阶段三的算法)
void DrawPixel(int x, int y, char ch);
void DrawLine(int x1, int y1, int x2, int y2, char ch);
void DrawRectangle(int x, int y, int width, int height, char ch);
void DrawCircle(int centerX, int centerY, int radius, char ch);
void DrawText(const char* text, int x, int y);

// 输入检测
bool IsKeyPressed(int key);

#endif