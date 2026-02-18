#pragma once

#include "GameLogic.h"
#include "GameView.h"
#include <stdio.h>
#include <assert.h>
#include <cstring>
#include "AIController.h"
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
void TestAI() {
    GameContext ctx;
    GameLogic logic;
    AIController ai;

    logic.Reset(ctx);
    // 构造一个简单的残局：左边高，右边低
    for(int y=15; y<20; y++)
        for(int x=0; x<5; x++) ctx.board[y][x] = 1;

    // 让 AI 思考
    ai.Think(ctx, logic);

    // 验证 AI 是否倾向于把方块放在右边（低处）
    if (ctx.curX > 5) {
        // AI 表现正常
    }
}
void TestAIPathfinding() {
    GameContext ctx;
    GameLogic logic;
    AIController ai;

    logic.Reset(ctx);
    ctx.curType = 1; // 假设是 I 型

    // 手动构造一个简单的地基
    for(int x=0; x<9; x++) ctx.board[19][x] = 1; // 留最右边一个洞

    ai.Think(ctx, logic);

    // 预期结果：AI 应该能算出最优 X 为 9 (填那个洞)
    if (ctx.curX == 9) {
        // AI 决策正确
    }
}
void testing()
{
    TestAI() ;
    TestAIPathfinding();
}

