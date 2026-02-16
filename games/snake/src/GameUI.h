// GameUI.h
#pragma once
#include "ray_console.h"
#include "SnakeGame.h"
#include <string>

class GameUI {
public:
    void Draw(const SnakeGame& game) {

        // 提示：现在 ClearBackground() 会把所有像素重置为默认色
        // 红色食物
        DrawPixelEx(game.getFood().x, game.getFood().y, '*', CG_COLOR_RED);

        // 绿色蛇身
        for (auto& p : game.getSnake()) {
            DrawPixelEx(p.x, p.y, 'O', CG_COLOR_GREEN);
        }

        // 黄色分数
        DrawTextEx(1, 1, ("Score: " + std::to_string(game.getScore())).c_str(), CG_COLOR_YELLOW);

        if (game.IsGameOver()) {
            // 遍历屏幕所有像素，把空白处换成红色的 '.'
            for (int y = 0; y < GetScreenHeight(); y++) {
                for (int x = 0; x < GetScreenWidth(); x++) {
                    // 这里可以做一个随机概率，让屏幕看起来像在崩塌
                    if (rand() % 10 == 0) DrawPixelEx(x, y, '.', CG_COLOR_RED);
                }
            }
            DrawText(" GAME OVER! Press ESC to Quit ", GetScreenWidth()/2 - 15, GetScreenHeight()/2);
            DrawTextEx(GetScreenWidth()/2 - 10, GetScreenHeight()/2, " [ SYSTEM FAILURE ] ", CG_COLOR_RED);
        }


    }


};