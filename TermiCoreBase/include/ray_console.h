// include/ray_console.h
#ifndef RAY_CONSOLE_H
#define RAY_CONSOLE_H

#include <stdbool.h>
#include "cg_gfx.h"


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
void DrawText(int x, int y,const char* text);

// 输入检测
bool IsKeyPressed(int key);
int GetScreenWidth(void);
int GetScreenHeight(void);
void SetTargetFPS(int fps);
int GetKeyPressed(void);

// include/ray_console.h
// 必须放在文件顶部，所有函数之外

void DrawPixelEx(int x, int y,  char ch,const char* color);
void DrawTextEx( int x, int y, const char* text,const char* color);


void PlayConsoleSound(void);
void SignalHandler(int sig);
#endif