#include "SnakeGame.h"
#include <cstdlib>
#include "ParticleSystem.hpp"
#include <iostream>
    // 那么构造函数必须写成：
SnakeGame::SnakeGame(int w, int h) : m_width(w), m_height(h), gameOver(false), score(0), dir(Direction::RIGHT)
{
        snake.push_front({w/2, h/2}); // 初始蛇头
        SpawnFood();

}

void SnakeGame::HandleInput(Direction newDir) {
        // 防止180度掉头
        if (dir == Direction::UP && newDir == Direction::DOWN) return;
        if (dir == Direction::DOWN && newDir == Direction::UP) return;
        if (dir == Direction::LEFT && newDir == Direction::RIGHT) return;
        if (dir == Direction::RIGHT && newDir == Direction::LEFT) return;
        dir = newDir;
}

bool SnakeGame::Update() {
    if (gameOver) return false;
    Point head = snake.front();

    // 移动逻辑
    if (dir == Direction::UP) head.y--;
    else if (dir == Direction::DOWN) head.y++;
    else if (dir == Direction::LEFT) head.x -= 2;
    else if (dir == Direction::RIGHT) head.x += 2;

    // --- 精准碰撞判定 ---
    // 判定条件必须和 DrawAt 画墙的坐标完全重合！
    // 如果墙画在 0 和 m_width-1，那么撞到这两个坐标就死
    if (head.x <= 0 || head.x >= m_width - 1 || head.y <= 0 || head.y >= m_height - 1) {
        gameOver = true;
        return false;
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

void SnakeGame::SpawnFood() {
    int margin = 2;
    bool valid = false;

    // 增加一个循环，直到找到不在蛇身上的空位
    while (!valid) {
        food.x = margin + (rand() % ((m_width - 4) / 2)) * 2;
        food.y = margin + (rand() % (m_height - 4));

        valid = true;
        // 检查是否在蛇身上
        for (const auto& p : snake) {
            if (p.x == food.x && p.y == food.y) {
                valid = false;
                break;
            }
        }
        // 如果地图快满了，为了防止死循环，尝试一定次数后强制跳出
        // 但对于目前的小窗口，这个循环是必须的
    }
}
void SnakeGame::SyncScreenSize(int w, int h) {
    if (w == m_width && h == m_height) return; // 没变就不动

    // 如果缩放后食物在墙外，强制把食物拉回到新边界内
    if (food.x >= w - 1) food.x = (w / 2);
    if (food.y >= h - 1) food.y = (h / 2);

    // 同样，如果蛇头出界了，只能判定游戏结束，防止程序崩溃
    Point head = snake.front();
    if (head.x >= w - 1 || head.y >= h - 1) {
        gameOver = true;
    }

    m_width = w;
    m_height = h;
}

int SnakeGame::GetFrameDelay() const {
    const int INITIAL_DELAY = 10; // 初始速度（每 10 帧动一次）
    const int MIN_DELAY = 2;      // 极限速度（每 2 帧动一次）
    const int SCORE_STEP = 50;    // 每得多少分加速一次

    // 计算延迟：每 50 分减少 1 帧延迟
    int delay = INITIAL_DELAY - (score / SCORE_STEP);

    // 边界检查：确保不会减到 0 或负数导致程序逻辑飞掉
    if (delay < MIN_DELAY) return MIN_DELAY;

    return delay;
}

void SnakeGame::Reset(int w, int h) {
    // 1. 状态清理
    snake.clear();
    score = 0;
    gameOver = false;
    dir = Direction::RIGHT; // 默认朝向
    // 只有当当前分数超过最高分时才更新
    if (m_score > m_highScore) {
        m_highScore = m_score;
    }
    m_score = 0; // 清空当前分，准备下一局
    // 2. 重新同步边界
    m_width = w;
    m_height = h;

    // 3. 初始蛇身位置（例如放在屏幕中心）
    int startX = (w / 4) * 2; // 确保步长对齐
    int startY = h / 2;

    // 初始化三个节段：头、中、尾
    snake.push_back({startX, startY});
    snake.push_back({startX - 2, startY});
    snake.push_back({startX - 4, startY});

    // 4. 生成第一个食物
    SpawnFood();
}
bool SnakeGame::IsSnakeAt(int x, int y) const {
    for (auto& p : snake) {
        if (p.x == x && p.y == y) return true;
    }
    return false;
}
