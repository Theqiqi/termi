#ifndef SNAKE_AI_H
#define SNAKE_AI_H

#include "SnakeGame.h"
#include "SnakeAI.h"
#include <queue>
#include <set>
#include <vector>
#include <cmath>
#include <algorithm>


class SnakeAI {
public:
    // 核心接口：输入当前游戏状态，输出最优方向
    static Direction CalculateEscapeMove(const SnakeGame& game);
    // 辅助函数：检查坐标是否合法且不是蛇身
    static bool IsValidMove(const SnakeGame& game, int x, int y);

    static Direction CalculateFastMove(const SnakeGame& game);
    static Direction CalculateBestMove(const SnakeGame& game);

private:
    struct Node {
        int x, y;
        std::vector<Direction> path;
    };
    // 内部结构：定义动作
    struct AIMove {
        Direction d;
        int dx, dy;
    };

    // 内部结构：搜索节点
    struct AINode {
        int x, y;
        std::vector<Direction> path;
    };
    static std::deque<Point> SimulateMove(const std::deque<Point>& currentSnake, Direction dir);
    static  std::vector<Direction> BFS(const SnakeGame& game, Point start, Point target, const std::deque<Point>& virtualBody) ;
    static Direction Wander(const SnakeGame& game);
    static Direction WanderSafely(const SnakeGame& game);
    static std::vector<Direction> SearchPath(const SnakeGame& game, Point start, Point target, const std::deque<Point>& obstacleBody);
    static std::deque<Point> SimulateSnakeMove(const std::deque<Point>& currentSnake, Direction dir);
};

#endif