// AIController.cpp

#include "AIController.h"
#include <memory>
#include "GameLogic.h"
#include <cmath>
#include <cstring>
#include "LogSystem.h"

void AIController::Think(GameContext& ctx, GameLogic& logic) {
    if (ctx.lineClearTimer > 0 || ctx.isGameOver) return;

    // 诊断日志：记录当前 AI 试图操作的方块类型
    LogSystem::Log("AI Thinking Start. PieceType: " + std::to_string(ctx.curPieceType));

    BestMove bestMove = { -1, -1, -1e9 };

    for (int r = 0; r < 4; r++) {
        // 缩小 x 范围进行测试，避免日志爆炸
        for (int x = 0; x < 7; x++) {
            int testBoard[20][10];
            memcpy(testBoard, ctx.board, sizeof(testBoard));

            // 调用模拟
            int finalY = logic.SimulateDrop(ctx, x, r, testBoard);

            if (finalY != -1) {
                double score = EvaluateBoard(testBoard);
                if (score > bestMove.score) {
                    bestMove.score = score;
                    bestMove.x = x;
                    bestMove.rotation = r;
                }
            } else {
                // 重点：如果中间位置 (x=3) 都模拟失败，打印原因
                if (x == 3 && r == 0) {
                    LogSystem::Log("AI: x=3 r=0 failed at y=0. Check Collision!");
                }
            }
        }
    }

    if (bestMove.x != -1) {
        // 埋点 3：找到结果
        LogSystem::Log("AI: Found Move X=" + std::to_string(bestMove.x));

        ctx.curX = bestMove.x;
        ctx.curRotation = bestMove.rotation;
        ctx.curY = 0;

        logic.HardDrop(ctx);

        // 埋点 4：HardDrop 完成
        LogSystem::Log("AI: HardDrop Done");
    } else {
        // 埋点 5：找不到结果
        LogSystem::Log("AI: No Valid Move Found!");
    }
}

double AIController::EvaluateBoard(const int board[20][10]) {
    int holes = CountHoles(board);
    int bumpiness = GetBumpiness(board);
    int aggregateHeight = 0;
    for (int x = 0; x < 10; x++) aggregateHeight += GetColumnHeight(board, x);

    // 假设你还统计了消行数 (linesCleared)
    int lines = 0; // 如果模拟逻辑里能拿到消行数更好
    // 技巧：在评估之前，计算这个棋盘有多少行是满的
    int clearedLines = 0;
    for (int y = 0; y < 20; y++) {
        bool full = true;
        for (int x = 0; x < 10; x++) {
            if (board[y][x] == 0) { full = false; break; }
        }
        if (full) clearedLines++;
    }
    // 经典启发式公式 (建议权重):
    // 分数 = (消行 * 0.76) - (总高度 * 0.51) - (空洞 * 0.35) - (平整度 * 0.18)
    // 更新后的公式：给消行极大的奖励
    return (clearedLines * 10.0)
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

