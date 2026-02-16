#include "GameUI.h"
#include "SnakeGame.h"
#include "SnakeAI.h"

bool fastMode = false;
void SetupGame(SnakeGame& logic) {
    // 确保拿到真实的终端大小
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    // 如果库还没准备好大小，给一个保底
    if (w <= 0) w = 80;
    if (h <= 0) h = 24;

    logic.Reset(w, h); // 使用你刚写的 Reset 函数来初始化一切
}
void ProcessGameInput(SnakeGame& logic, bool& isPaused, bool& aiMode) {
    int key = GetKeyPressed();
    if (key == 'f' || key == 'F') fastMode = !fastMode;
    if (key == 'p' || key == 'P') isPaused = !isPaused;
    if (key == 'm' || key == 'M') aiMode = !aiMode;

    if (logic.IsGameOver() && (key == 'r' || key == 'R')) {
        logic.Reset(GetScreenWidth() - 4, GetScreenHeight() - 3);
        return;
    }

    // 只有在【非AI模式】下才处理 WASD
    if (!isPaused && !logic.IsGameOver() && !aiMode) {
        if (IsKeyPressed('w')) logic.HandleInput(Direction::UP);
        if (IsKeyPressed('s')) logic.HandleInput(Direction::DOWN);
        if (IsKeyPressed('a')) logic.HandleInput(Direction::LEFT);
        if (IsKeyPressed('d')) logic.HandleInput(Direction::RIGHT);
    }
}


/**
 * 运行一帧游戏逻辑
 * @param logic 引用游戏逻辑对象
 * @param ps 引用粒子系统，用于吃到食物时发射粒子
 * @param moveCounter 引用主循环中的计数器，必须传引用以保持状态持久化
 */
/**
 * 运行一帧游戏逻辑
 */
void RunGameLogicFrame(SnakeGame& logic, ParticleSystem& ps, int& moveCounter, bool aiMode) {
    if (fastMode) moveCounter += 5;
    else moveCounter += 1;

    if (moveCounter >= logic.GetFrameDelay()) {
        // --- AI 模式下的唯一决策点 ---
        if (aiMode) {
            // 这里调用你测试通过的 CalculateBFSMove
            logic.HandleInput(SnakeAI::CalculateBestMove(logic));
        }

        bool ateSomething = logic.Update();
        if (ateSomething) {
            ps.Emit((float)logic.GetHeadX(), (float)logic.GetHeadY(), 15, CG_COLOR_YELLOW);
        }
        moveCounter = 0;
    }
}

void DrawHUD(const SnakeGame& logic, bool aiMode) {
    // 准备显示文本
    std::string modeText = aiMode ? " [AI PILOT] " : " [MANUAL] ";
    const char* modeColor = aiMode ? CG_COLOR_GREEN : CG_COLOR_CYAN;

    // 渲染模式
    DrawTextEx(2, 0, modeText.c_str(), modeColor);

    // 渲染分数（位置稍微偏移，避免重叠）
    std::string scoreStr = "SCORE: " + std::to_string(logic.GetScore()) +
                           "  HIGH: " + std::to_string(logic.GetHighScore());
    DrawTextEx(20, 0, scoreStr.c_str(), CG_COLOR_YELLOW);
}
void DrawControls(int screenHeight) {
    // 放在屏幕最后一行
    const char* hint = " [P]Pause  [M]AI  [F]Fast  [R]Reset  [ESC]Exit ";
    DrawTextEx(2, screenHeight - 1, hint, CG_COLOR_GRAY);
}
void DrawGameOverScreen(const SnakeGame& logic, GameUI& view) {
    std::string msg = "\n   === MISSION FAILED ===\n\n";
    msg += "   FINAL SCORE : " + std::to_string(logic.GetScore()) + "\n";
    msg += "   BEST RECORD : " + std::to_string(logic.GetHighScore()) + "\n\n";
    msg += "   PRESS 'R' TO REBOOT SYSTEM";

    view.DrawOverlay(msg.c_str(), logic);
}
/**
 * 执行完整的渲染序列
 */
void RenderFrame(const SnakeGame& logic, GameUI& view, ParticleSystem& ps, bool isPaused, bool aiMode) {
    BeginDrawing();
    ClearBackground(); // 必须先清屏

    // 1. 顶部：信息区
    DrawHUD(logic, aiMode);

    // 2. 中间：游戏核心区（墙、蛇、食物）
    view.Draw(logic);

    // 3. 特效层：粒子更新与渲染
    ps.Update();
    ps.Render();

    // 4. 底部：指令指引
    // 使用逻辑高度作为基准，或者直接用 GetScreenHeight()
    DrawControls(GetScreenHeight());

    // 5. 顶层：状态遮罩（仅在结束或暂停时显示）
    if (logic.IsGameOver()) {
        DrawGameOverScreen(logic, view);
    } else if (isPaused) {
        view.DrawOverlay("\n   SYSTEM PAUSED\n\n   PRESS 'P' TO RESUME", logic);
    }

    EndDrawing();
}
// main.cpp
int main() {
    InitConsole(0, 0);
    SetTargetFPS(60);

    // 1. 启动时获取一次大小
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    // 给边缘留出安全余量
    int logicW = sw - 6;
    int logicH = sh - 4;

    SnakeGame logic(logicW, logicH);
    logic.Reset(logicW, logicH);

    GameUI view;
    ParticleSystem ps;

    int moveCounter = 0;
    bool isPaused = false;
    bool aiMode = false;

    while (!ConsoleWindowShouldClose()) {
        if (IsKeyPressed(27)) break;

        // --- 恢复动态同步：这是让墙壁随窗口动的关键 ---
        int realW = GetScreenWidth();
        int realH = GetScreenHeight();
        // 逻辑大小永远紧跟物理窗口
        logic.SyncScreenSize(realW - 4, realH - 3);

        ProcessGameInput(logic, isPaused, aiMode);

        if (!isPaused && !logic.IsGameOver()) {
            // AI 的指令下达
            if (aiMode && (moveCounter >= logic.GetFrameDelay())) {
                // 这里的接口名要和你 SnakeAI 里的对上
                logic.HandleInput(SnakeAI::CalculateFastMove(logic));
            }
            RunGameLogicFrame(logic, ps, moveCounter,aiMode);
        }

        RenderFrame(logic, view, ps, isPaused, aiMode);
    }

    CloseConsole();
    return 0;
}