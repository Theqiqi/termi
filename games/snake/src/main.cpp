#include "GameUI.h"
#include "SnakeGame.h"


// main.cpp
int main() {
    InitConsole(0, 0);
    SetTargetFPS(60); // 渲染帧率保持高频

    SnakeGame logic(GetScreenWidth(), GetScreenHeight());
    GameUI view;

    int moveCounter = 0;
    ParticleSystem ps;
    while (!ConsoleWindowShouldClose()) {
        int key = GetKeyPressed();

        // 27 是 Esc 的 ASCII 码
        if (key == 'q' || key == 'Q' || key == 27) {
            break;
        }
        // 1. 输入处理 (毫秒级响应)
        if (IsKeyPressed('w')) logic.HandleInput(Direction::UP);
        if (IsKeyPressed('s')) logic.HandleInput(Direction::DOWN);
        if (IsKeyPressed('a')) logic.HandleInput(Direction::LEFT);
        if (IsKeyPressed('d')) logic.HandleInput(Direction::RIGHT);

        // 2. 逻辑更新 (分频执行，控制蛇速)
        // main.cpp 循环内部

        // 计算当前难度下的帧间隔
        // 初始是 10 帧移动一次。每得 50 分，间隔减少 1 帧，最快减到 2 帧移动一次。
        int currentScore = logic.getScore();
        int frameDelay = 10 - (currentScore / 50);
        if (frameDelay < 2) frameDelay = 2; // 设置一个极限速度，否则减到 0 游戏就没法玩了

        // 2. 逻辑更新 (使用动态的 frameDelay)
        if (++moveCounter >= frameDelay) {
            bool ateSomething = logic.Update();
            if (ateSomething) {
                // 速度越快（frameDelay 越小），粒子的数量越多，飞得越快
                int intensity = 15 + (10 - frameDelay) * 5;
                ps.Emit((float)logic.GetHeadX(), (float)logic.GetHeadY(), intensity, CG_COLOR_YELLOW);
            }
            moveCounter = 0;
        }

        // 3. 粒子系统每帧更新（60FPS），确保飞散过程平滑
        ps.Update();
        BeginDrawing();
        // 4. 渲染
        view.Draw(logic);
        ps.Render();      // 粒子最后画，确保它们飘在蛇和墙的上方
        EndDrawing();
    }

    CloseConsole();
    return 0;
}