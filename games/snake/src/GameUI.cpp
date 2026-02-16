#include "ray_console.h"
#include "GameUI.h"
void GameUI::Draw(const SnakeGame& game) {
    // 1. 物理清屏：覆盖整个终端
    for(int y=0; y < GetScreenHeight(); y++) {
        for(int x=0; x < GetScreenWidth(); x++) {
            DrawPixelEx(x, y, ' ', CG_COLOR_BLACK);
        }
    }

    const int START_X = 4;
    const int START_Y = 2;
    int w = game.GetWidth();
    int h = game.GetHeight();

    auto DrawAt = [&](int x, int y, char s, const char* c) {
        DrawPixelEx(x + START_X, y + START_Y, s, c);
    };

    // 2. 绘制墙壁 (0 到 h-1)
    for (int x = 0; x < w; x++) {
        DrawAt(x, 0, '#', CG_COLOR_GRAY);
        DrawAt(x, h - 1, '#', CG_COLOR_GRAY);
    }
    for (int y = 0; y < h; y++) {
        DrawAt(0, y, '#', CG_COLOR_GRAY);
        DrawAt(w - 1, y, '#', CG_COLOR_GRAY);
    }
    //画分数
    std::string scoreStr = "Score: " + std::to_string(game.GetScore());
    DrawTextEx(1, 1, scoreStr.c_str(), CG_COLOR_YELLOW);

    // 2. 画食物
    DrawAt(game.getFood().x, game.getFood().y, '$', CG_COLOR_RED);

    // 3. 画蛇
    for (const auto& p : game.getSnake()) {
        DrawAt(p.x, p.y, 'O', CG_COLOR_GREEN);
    }

    // 4. 死亡特效：确保它也遵循偏移逻辑
    if (game.IsGameOver()) {
        this->RenderDeathEffect(w, h);
    }
}


// 专门负责死亡特效
void GameUI::RenderDeathEffect(int w, int h) {
    const int START_X = 4;
    const int START_Y = 2;
    for (int i = 0; i < 50; i++) {
        int rx = rand() % w;
        int ry = rand() % h;
        // 关键：特效也要加上偏移量，否则红点会飞到围墙外面
        DrawPixelEx(rx + START_X, ry + START_Y, '.', CG_COLOR_RED);
    }
}
/**
 * 在屏幕中央绘制一个半透明遮罩并显示提示文字
 * @param text 要显示的文字内容
 */
void GameUI::DrawOverlay(const std::string& text, const SnakeGame& game) {
    int screenW = game.GetWidth();
    int screenH = game.GetHeight();

    // 1. 绘制一个简单的半透明区域（如果你的库支持 Alpha，否则可以用特殊字符模拟）
    // 这里我们先在文字周围画一个小框，或者直接在中央定位
    int centerX = screenW / 2;
    int centerY = screenH / 2;

    // 2. 计算文字偏移量，使其居中
    int textX = centerX - (int)(text.length() / 2);
    int textY = centerY;

    // 3. 调用你的库函数画文字
    // 假设你的库有 DrawText(text, x, y, color) 这种方法
    DrawTextEx( textX, textY, text.c_str(),CG_COLOR_WHITE);

    // 如果想要更美观，可以在文字下面画一行装饰线
    std::string decoration(text.length(), '-');
    DrawTextEx( textX, textY + 1, decoration.c_str(),CG_COLOR_GRAY);
}


void GameUI::DrawScore(const SnakeGame& game) {
    // 黄色分数，固定在左上角 (1, 1)
    std::string scoreStr = "Score: " + std::to_string(game.GetScore());
    DrawTextEx(1, 1, scoreStr.c_str(), CG_COLOR_YELLOW);
}

