#include <iostream>
#include <chrono>
#include <thread>
#include "cg_gfx.h"
#include <cmath>

#include "ray_console.h"

int main() {
    InitConsole(0, 0);

    int posX = 10;
    while (!ConsoleWindowShouldClose()) {
        BeginDrawing();
        ClearBackground();

        // 静态背景
        DrawRectangle(2, 2, 20, 5, '#');
        DrawText("ENGINE READY", 5, 4);

        // 动态元素
        DrawCircle(posX, 15, 4, '@');

        // 输入处理
        if (IsKeyPressed('d')) posX++;
        if (IsKeyPressed('a')) posX--;
        if (IsKeyPressed('q')) break;

        EndDrawing();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    CloseConsole();
    return 0;
}