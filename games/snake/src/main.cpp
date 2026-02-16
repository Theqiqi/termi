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

        if (++moveCounter >= 10) {
            // 1. 执行逻辑更新，同时得知是否吃到东西
            bool ateSomething = logic.Update();

            // 2. 如果吃到了，就在蛇头当前位置（即食物消失处）放烟火
            if (ateSomething) {
                // 这里的 15 是粒子数量，'.' 是粒子形状
                ps.Emit((float)logic.GetHeadX(), (float)logic.GetHeadY(), 15, CG_COLOR_YELLOW);

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