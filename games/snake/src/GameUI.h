// GameUI.h
#pragma once

#include "SnakeGame.h"
#include <string>

class GameUI {
public:
    void Draw(const SnakeGame& game);
    // 专门负责死亡特效
    void RenderDeathEffect(int screenW, int screenH);
    /**
     * 在屏幕中央绘制一个半透明遮罩并显示提示文字
     * @param text 要显示的文字内容
     */
    void DrawOverlay(const std::string& text, const SnakeGame& game);
    void DrawPixelWithOffset(int x, int y, char symbol, const char* color);
private:
    void DrawWalls(const SnakeGame& game);
    void DrawFood(const SnakeGame& game);
    void DrawSnake(const SnakeGame& game);
    void DrawScore(const SnakeGame& game);
};