#include "../src/SnakeGame.h"
#include <cassert>
#include <iostream>

void TestCollision() {
    std::cout << "Running TestCollision..." << std::endl;
    SnakeGame game(20, 20); // 内部 m_width=20, m_height=20

    // 1. 模拟撞左墙：逻辑坐标 0 是墙
    game._test_SetSnakeHead(2, 10); // 放在靠近边缘的位置
    game._test_SetDirection(Direction::LEFT);

    // 第一次移动：2 -> 0
    game.Update();
    // 第二次移动：0 -> -2 (触发判定 head.x <= 0)
    game.Update();

    assert(game.IsGameOver() == true);
    std::cout << "Left collision test passed!" << std::endl;

    // 2. 模拟撞底墙：逻辑坐标 19 是墙 (m_height - 1)
    SnakeGame game2(20, 20);
    game2._test_SetSnakeHead(10, 18);
    game2._test_SetDirection(Direction::DOWN);
    game2.Update(); // 18 -> 19 (触发 head.y >= m_height - 1)

    assert(game2.IsGameOver() == true);
    std::cout << "Bottom collision test passed!" << std::endl;
}
void TestScoring() {
    std::cout << "Running TestScoring..." << std::endl;
    SnakeGame game(20, 20); // 保持固定大小，不要 Sync

    int startX = 10;
    int startY = 10;
    int step = 2; // 左右步长是 2

    game._test_SetSnakeHead(startX, startY);
    game._test_SetDirection(Direction::RIGHT);
    game._test_SetFood(startX + step, startY); // 确保坐标完全重合

    int oldScore = game.GetScore();
    game.Update();

    assert(game.GetScore() > oldScore);
    std::cout << "Scoring test passed! New Score: " << game.GetScore() << std::endl;
}
void TestSuicide() {
    std::cout << "Running TestSuicide..." << std::endl;
    SnakeGame game(20, 20);

    // 构造蛇身，确保 X 坐标都是偶数 (0, 2, 4...)
    std::vector<Point> mockBody = {
        {10, 10}, // 头
        {10, 11}, // 颈
        {8, 11},  // 身
        {8, 10}   // 尾
    };
    game._test_SetSnakeBody(mockBody);
    game._test_SetDirection(Direction::LEFT); // 10 向左移动 2 步到 8

    game.Update();

    assert(game.IsGameOver() == true);
    std::cout << "Suicide test passed!" << std::endl;
}
void TestAlignment() {
    SnakeGame game(40, 20);
    for(int i = 0; i < 50; i++) {
        game._test_TriggerSpawnFood();
        // 核心断言：食物的 X 坐标必须是偶数
        assert(game.GetFoodX() % 2 == 0);
    }
    std::cout << "Alignment test passed: Food is always on even X coords." << std::endl;
}
void TestScreenFull() {
    std::cout << "Running TestScreenFull (Safety Test)..." << std::endl;

    int width = 10;
    int height = 10;
    SnakeGame game(width, height);

    // 1. 构造一个几乎填满屏幕的蛇
    std::vector<Point> fullBody;
    for (int y = 2; y < height - 2; ++y) {
        for (int x = 2; x < width - 2; x += 2) {
            fullBody.push_back({x, y});
        }
    }
    game._test_SetSnakeBody(fullBody);

    // 2. 执行生成
    // 这里不再担心死循环，测试会瞬间通过
    game._test_TriggerSpawnFood();

    // 3. 检查边界：确保食物至少没刷到墙上
    assert(game.GetFoodX() >= 2 && game.GetFoodX() < width - 2);
    assert(game.GetFoodY() >= 2 && game.GetFoodY() < height - 2);

    std::cout << "Safety test passed! No blocking logic found." << std::endl;
}