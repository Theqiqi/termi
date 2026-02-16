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
/**
 * 执行完整的渲染序列
 */
void RenderFrame(const SnakeGame& logic, GameUI& view, ParticleSystem& ps, bool isPaused, bool aiMode) {

    // 所有的渲染必须包裹在 Begin/End 之间
    BeginDrawing();
    ClearBackground();
    // 1. 底层：游戏主体（食物、蛇、墙、分数、死亡特效）
    view.Draw(logic);

    // 2. 中层：如果游戏结束，注入崩塌粒子
    if (logic.IsGameOver()) {
        ps.EmitDeathExplosion(logic.GetWidth(), logic.GetHeight());
    }

    // 3. 粒子层：更新并渲染所有活跃粒子
    ps.Update();
    ps.Render();

    // 4. 顶层 UI：状态遮罩和文字提示
    if (logic.IsGameOver()) {
        view.DrawOverlay(" [ SYSTEM FAILURE ] \n PRESS R TO REBOOT ", logic);
    } else if (isPaused) {
        view.DrawOverlay(" SYSTEM PAUSED ", logic);
    }

    // 5. 调试/模式信息（最前方显示）
    if (aiMode && !isPaused) {
        DrawTextEx(10, 10, "AI PILOT ACTIVE", CG_COLOR_GREEN);
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