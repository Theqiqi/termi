#pragma once

#include "GameLogic.h"
#include "GameView.h"
#include <stdio.h>
#include <assert.h>
#include <cstring>
void Test_UI_Coordinate_Mapping() {
    GameView view;
    int sx, sy;

    // 测试棋盘左上角
    view.MapLogicToScreen(0, 0, sx, sy);
    assert(sx == 2); // 假设 BOARD_OFFSET_X = 2
    assert(sy == 1); // 假设 BOARD_OFFSET_Y = 1

    // 测试棋盘右下角 (9, 19)
    view.MapLogicToScreen(9, 19, sx, sy);
    // 控制台通常一个字符格是 1x1，但有的库画方块用两个字符（如 "[]"）
    // 如果你是一个坐标对应一个字符：
    assert(sx == 2 + 9);
    assert(sy == 1 + 19);

    printf("[PASS] UI 坐标映射测试通过。\n");
}
void Test_UI_Color_Logic() {
    GameView view;
    // 假设 1 是 I 型，对应颜色是 Cyan
    assert(strcmp(view.GetColorByType(1), CG_COLOR_CYAN) == 0);
    // 测试边界值
    assert(view.GetColorByType(0) != nullptr);
    printf("[PASS] UI 颜色分配测试通过。\n");
}
void testing()
{

}

