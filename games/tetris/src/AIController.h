// AIController.h
#pragma once
#include "GameContext.h"
#include "GameLogic.h"

struct BestMove {
    int x;          // 目标 X 坐标
    int rotation;   // 目标旋转角度
    double score;   // 该位置的评分
};

struct AIDecision {
    int targetX;
    int targetRotation;
    bool isValid = false;
};

class AIController {
public:
    // 核心接口：思考并行动
    AIDecision Think(GameContext& ctx, GameLogic& logic);

private:
    // 计算某个残局的分数
    double EvaluateBoard(const int board[20][10]);

    // 获取某一列的高度
    int GetColumnHeight(const int board[20][10], int x);

    // 统计空洞数量
    int CountHoles(const int board[20][10]);

    // 统计平整度（高度差）
    int GetBumpiness(const int board[20][10]);
    bool IsPathClear(const GameContext& ctx, GameLogic& logic, int targetX, int targetR);
    bool IsPathReachable(const GameContext& ctx, GameLogic& logic, int targetX, int targetR);
};