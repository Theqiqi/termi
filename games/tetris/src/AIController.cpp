// AIController.cpp

#include "AIController.h"
#include <memory>
#include "GameLogic.h"
#include <cmath>
#include <cstring>
#include "LogSystem.h"



// AIController.cpp

AIDecision AIController::Think(GameContext& ctx, GameLogic& logic) {
    if (ctx.lineClearTimer > 0 || ctx.isGameOver) return { 0, 0, false };

    BestMove bestMove = { -1, -1, -1e9 };
    bool found = false;

    for (int r = 0; r < 4; r++) {
        // 这里的循环范围一定要固定，不要写变量
        for (int x = -2; x <= 8; x++) {
            // 路径检查增加强制退出机制
            if (!IsPathReachable(ctx, logic, x, r)) continue;

            int testBoard[20][10];
            memcpy(testBoard, ctx.board, sizeof(testBoard));

            int finalY = logic.SimulateDrop(ctx, x, r, testBoard);
            if (finalY != -1) {
                double score = EvaluateBoard(testBoard);
                if (score > bestMove.score) {
                    bestMove.score = score;
                    bestMove.x = x;
                    bestMove.rotation = r;
                    found = true;
                }
            }
        }
    }

    return { bestMove.x, bestMove.rotation, found };
}

bool AIController::IsPathReachable(const GameContext& ctx, GameLogic& logic, int targetX, int targetR) {
    // 1. 检查目标位置在 y=0 是否合法
    if (!logic.IsPositionValid(ctx.curPieceType, targetX, 0, targetR, ctx.board)) return false;

    // 2. 模拟横移过程
    int startX = 3;
    int dir = (targetX > startX) ? 1 : -1;
    int currX = startX;

    // 限制最大步数为棋盘宽度，绝对防止死循环
    for (int i = 0; i < 12; i++) {
        if (currX == targetX) return true;
        currX += dir;
        if (!logic.IsPositionValid(ctx.curPieceType, currX, 0, targetR, ctx.board)) return false;
    }
    return false;
}
// AIController.cpp 里的 EvaluateBoard 函数
double AIController::EvaluateBoard(const int board[20][10]) {
    int holes = 0;
    int aggregateHeight = 0;
    int bumpiness = 0;
    int rowTransitions = 0;
    int colTransitions = 0;
    int clearedLines = 0;

    // 1. 计算消行 (奖励)
    for (int y = 0; y < 20; y++) {
        bool full = true;
        for (int x = 0; x < 10; x++) if (board[y][x] == 0) { full = false; break; }
        if (full) clearedLines++;
    }

    // 2. 统计高度和空洞
    int colHeights[10] = {0};
    for (int x = 0; x < 10; x++) {
        bool blockFound = false;
        for (int y = 0; y < 20; y++) {
            // 列变换：检测垂直方向上 方块->空格 的切换次数
            if (y > 0 && (board[y][x] > 0) != (board[y-1][x] > 0)) colTransitions++;

            if (board[y][x] > 0) {
                if (!blockFound) {
                    colHeights[x] = 20 - y;
                    blockFound = true;
                }
            } else if (blockFound && board[y][x] == 0) {
                // 核心：被盖住的空洞，惩罚最重
                holes++;
            }
        }
        aggregateHeight += colHeights[x];
    }

    // 3. 计算行变换与平整度
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 9; x++) {
            // 行变换：检测水平方向上 方块->空格 的切换次数
            if ((board[y][x] > 0) != (board[y][x+1] > 0)) rowTransitions++;
        }
    }

    for (int x = 0; x < 9; x++) {
        bumpiness += abs(colHeights[x] - colHeights[x + 1]);
    }

    // --- 严厉惩罚参数 ---
    double score = 0;
    score += (clearedLines * 500.0);      // 鼓励消行
    score += (aggregateHeight * -10.0);   // 控制高度
    score += (holes * -600.0);            // 致命惩罚：空洞 (从-350调到-600)
    score += (bumpiness * -40.0);         // 严厉惩罚：地面不平 (防止深井)
    score += (rowTransitions * -30.0);    // 惩罚：行碎块化
    score += (colTransitions * -30.0);    // 惩罚：列碎块化

    return score;
}
int AIController::CountHoles(const int board[20][10]) {
    int holes = 0;
    for (int x = 0; x < 10; x++) {
        bool coverFound = false;
        for (int y = 0; y < 20; y++) {
            if (board[y][x] > 0) {
                coverFound = true; // 发现“盖子”
            } else if (coverFound && board[y][x] == 0) {
                // 只要上方有盖子，当前格子又是空的，这就是一个空洞
                holes++;
            }
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
