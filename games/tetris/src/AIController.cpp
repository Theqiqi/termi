// AIController.cpp

#include "AIController.h"
#include <memory>
#include "GameLogic.h"
#include <cmath>
#include <cstring>

void AIController::Think(GameContext& ctx, GameLogic& logic) {
    BestMove bestMove = { -1, -1, -1000000.0 };

    // 1. 穷举 4 个旋转方向
    for (int r = 0; r < 4; r++) {
        // 2. 穷举每一列
        for (int x = -2; x < 10; x++) {
            // 模拟下落
            int testBoard[20][10];
            // 复制当前棋盘到模拟棋盘
            memcpy(testBoard, ctx.board, sizeof(testBoard));

            // 在模拟棋盘中尝试放置方块（这部分需要一个不修改 ctx 的逻辑函数）
            int finalY = logic.SimulateDrop(ctx, x, r, testBoard);

            if (finalY != -1) { // 如果位置合法
                double score = EvaluateBoard(testBoard);
                if (score > bestMove.score) {
                    bestMove.score = score;
                    bestMove.x = x;
                    bestMove.rotation = r;
                }
            }
        }
    }

    // 3. 执行指令：根据 bestMove 发送模拟按键
    if (bestMove.x != -1) {
        if (ctx.rotation != bestMove.rotation) {
            ctx.isWPressed = true; // 模拟旋转
        } else if (ctx.curX < bestMove.x) {
            ctx.isDPressed = true; // 向右移
        } else if (ctx.curX > bestMove.x) {
            ctx.isAPressed = true; // 向左移
        } else {
            ctx.isSpacePressed = true; // 到位，直接速降
        }
    }
}

double AIController::EvaluateBoard(const int board[20][10]) {
    int holes = CountHoles(board);
    int bumpiness = GetBumpiness(board);
    int aggregateHeight = 0;
    for (int x = 0; x < 10; x++) aggregateHeight += GetColumnHeight(board, x);

    // 假设你还统计了消行数 (linesCleared)
    int lines = 0; // 如果模拟逻辑里能拿到消行数更好

    // 经典启发式公式 (建议权重):
    // 分数 = (消行 * 0.76) - (总高度 * 0.51) - (空洞 * 0.35) - (平整度 * 0.18)
    return (lines * 0.76)
         + (aggregateHeight * -0.51)
         + (holes * -0.35)
         + (bumpiness * -0.18);
}
int AIController::CountHoles(const int board[20][10]) {
    int holes = 0;
    for (int x = 0; x < 10; x++) {
        bool blockFound = false;
        for (int y = 0; y < 20; y++) {
            if (board[y][x] > 0) blockFound = true;
            else if (blockFound && board[y][x] == 0) holes++; // 上方有方块，下方是空的，这就是空洞
        }
    }
    return holes;
}

int AIController::GetBumpiness(const int board[20][10]) {
    int bumpiness = 0;
    for (int x = 0; x < 9; x++) {
        bumpiness += abs(GetColumnHeight(board, x) - GetColumnHeight(board, x + 1));
    }
    return bumpiness;
}

int AIController::GetColumnHeight(const int board[20][10], int x) {
    // 从 y = 0 (顶部) 开始向下扫描
    for (int y = 0; y < 20; y++) {
        if (board[y][x] > 0) {
            // 如果在第 y 行找到了方块，那么高度就是 (总高度 20 - 当前行号 y)
            // 例如：方块在最底下一行 (y=19)，高度就是 1
            return 20 - y;
        }
    }
    return 0; // 该列是空的
}

