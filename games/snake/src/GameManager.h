#pragma once

#include "GameUI.h"
#include "SnakeGame.h"

class GameManager {
    SnakeGame logic;
    GameUI view;
public:
    void Run() {
        InitConsole(0, 0);
        SetTargetFPS(60); // 渲染高频，逻辑低频

        int frameCounter = 0;
        while (!ConsoleWindowShouldClose()) {
            // 1. 输入处理 (响应灵敏)
            if (IsKeyPressed('w')) logic.HandleInput(Direction::UP);
            if (IsKeyPressed('s')) logic.HandleInput(Direction::DOWN);
            if (IsKeyPressed('a')) logic.HandleInput(Direction::LEFT);
            if (IsKeyPressed('d')) logic.HandleInput(Direction::RIGHT);

            // 2. 逻辑更新 (控制蛇速：每 6 帧移动一次，即 10FPS)
            if (++frameCounter >= 6) {
                logic.Update();
                frameCounter = 0;
            }

            // 3. 渲染
            view.Render(logic);

            if (logic.IsGameOver() && IsKeyPressed('r')) {
                // 重置逻辑逻辑...
            }
        }
        CloseConsole();
    }
};