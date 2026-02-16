// SnakeGame.h
#pragma once
#include <deque>
#include <vector>
#include <cstdlib>
#include "ParticleSystem.hpp"
struct Point { int x, y; };
enum class Direction { UP, DOWN, LEFT, RIGHT };


class SnakeGame {
public:
    // 那么构造函数必须写成：
    SnakeGame(int w, int h) : width(w), height(h), gameOver(false), score(0), dir(Direction::RIGHT){
        snake.push_front({w/2, h/2}); // 初始蛇头
        SpawnFood();
    }

    void HandleInput(Direction newDir) {
        // 防止180度掉头
        if (dir == Direction::UP && newDir == Direction::DOWN) return;
        if (dir == Direction::DOWN && newDir == Direction::UP) return;
        if (dir == Direction::LEFT && newDir == Direction::RIGHT) return;
        if (dir == Direction::RIGHT && newDir == Direction::LEFT) return;
        dir = newDir;
    }

    bool Update() {
        if (gameOver) return false;

        // 1. 动态获取当前窗口大小（解决缩放后找不到边界的问题）
        int currentW = GetScreenWidth();
        int currentH = GetScreenHeight();

        Point head = snake.front();

        // 2. 视觉速度修正：左右步长设为 2，上下为 1
        // 这样蛇在屏幕上爬行的物理距离会趋于一致
        if (dir == Direction::UP) head.y--;
        else if (dir == Direction::DOWN) head.y++;
        else if (dir == Direction::LEFT) head.x -= 2;
        else if (dir == Direction::RIGHT) head.x += 2;

        // 3. 碰撞检测（使用动态宽高）
        if (head.x < 0 || head.x >= currentW || head.y < 0 || head.y >= currentH) {
            gameOver = true;
            return false ;
        }

        for (auto& p : snake) {
            if (p.x == head.x && p.y == head.y) { gameOver = true; return false; }
        }
        snake.push_front(head);
        // 4. 吃食物逻辑（由于步长变了，判定需要一点容差或让食物也生在偶数位）
        if (abs(head.x - food.x) <= 1 && head.y == food.y) {
            score += 10;
            PlayConsoleSound();
            SpawnFood();
            return true; // <--- 关键：返回 true 表示这一帧发生了爆炸事件
        } else {
            snake.pop_back();
        }
        return false; // 普通移动返回 false


    }

    void SpawnFood() {
        int w = GetScreenWidth();
        int h = GetScreenHeight();

        // 防御性编程：如果获取不到宽度，给一个默认值防止崩溃
        if (w <= 0) w = 40;
        if (h <= 0) h = 20;

        // 确保蛇能吃到（步长对齐）
        food.x = (rand() % (w / 2)) * 2;
        food.y = rand() % h;
    }

    // 只读接口供渲染层使用
    const std::deque<Point>& getSnake() const { return snake; }
    Point getFood() const { return food; }
    int getScore() const { return score; }
    bool IsGameOver() const { return gameOver; }

    // 获取蛇头坐标，用于粒子爆发的起始点
    int GetHeadX() const { return snake.front().x; }
    int GetHeadY() const { return snake.front().y; }

    // 或者如果你想在食物位置爆发，也可以提供这个
    int GetFoodX() const { return food.x; }
    int GetFoodY() const { return food.y; }

private:
    // SnakeGame.h
private:
    int width, height;
    std::deque<Point> snake;
    Point food;
    bool gameOver; // gameOver 在前
        int score = 0;     // score 在中
    Direction dir; // dir 在后
    // 2. 在 Snake 类或 UI 类中增加粒子容器
    std::vector<Particle> particles;

};