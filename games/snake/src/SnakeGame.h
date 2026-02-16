// SnakeGame.h
#pragma once
#include <deque>
#include <vector>
#include "ParticleSystem.hpp"


struct Point { int x, y; };
enum class Direction { UP, DOWN, LEFT, RIGHT };

class SnakeGame {
public:
    // 那么构造函数必须写成：
    SnakeGame(int w, int h);

    void HandleInput(Direction newDir) ;
    int GetWidth() const{return m_width;}
    int GetHeight() const{return m_height;};
    bool Update();

    void SpawnFood();

    // 只读接口供渲染层使用
    const std::deque<Point>& getSnake() const { return snake; }
    Point getFood() const { return food; }
    int GetScore() const { return score; }
    bool IsGameOver() const { return gameOver; }

    // 获取蛇头坐标，用于粒子爆发的起始点
    int GetHeadX() const { return snake.front().x; }
    int GetHeadY() const { return snake.front().y; }

    // 或者如果你想在食物位置爆发，也可以提供这个
    int GetFoodX() const { return food.x; }
    int GetFoodY() const { return food.y; }

    void SyncScreenSize(int w, int h);
    int GetFrameDelay() const;
    bool IsSnakeAt (int x, int y)const;
    int GetHighScore() const { return m_highScore; };
#ifdef GAME_DEBUG
    // 测试专用：强制设置蛇的位置
    void _test_SetSnakeHead(int x, int y) {
        if (!snake.empty()) snake.front() = {x, y};
    }
    // 测试专用：强制设置食物位置
    void _test_SetFood(int x, int y) {
        food.x = x; food.y = y;
    }
    // 测试专用：获取私有变量用于断言
    int _test_GetSnakeSize() { return snake.size(); }
    void _test_SetDirection(Direction newDir) {
        this->dir = newDir; // 确保 currentDir 是你类里存方向的变量名
    }
    // 强制设置整条蛇的坐标（从头到尾）
    void _test_SetSnakeBody(const std::vector<Point>& newBody) {
        snake.clear();
        for (const auto& p : newBody) {
            snake.push_back(p);
        }
    }
    // 获取当前蛇的长度
    int _test_GetSnakeLength() { return snake.size(); }

    // 强制触发食物生成，用于观察是否会死循环
    void _test_TriggerSpawnFood() { SpawnFood(); }

#endif
    void Reset(int w, int h);
public:
    int m_width = 800; // 默认值
    int m_height = 600;

private:
    std::deque<Point> snake;
    Point food;
    bool gameOver; // gameOver 在前
    int score = 0;     // score 在中
    int m_score = 0;
    int m_highScore = 0; // 新增：记录最高分
    Direction dir; // dir 在后
    // 2. 在 Snake 类或 UI 类中增加粒子容器
    std::vector<Particle> particles;
    struct GameState { std::deque<Point> snake; Point food; };

};