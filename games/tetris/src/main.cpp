#include <iostream>

#include "ray_console.h"  // 你的字符引擎封装
#include "GameContext.h"    // 纯数据结构
#include "GameLogic.h"      // 纯逻辑处理
#include "GameView.h"       // 字符渲染处理
#include "InputHandler.h"
#include "TimeTicker.h"

class AudioManager {
public:
    // 只传入 ctx 引用，纯读取，不修改
    void Update(const GameContext& ctx) {

        // 1. 捕捉消行瞬间
        if (ctx.lineClearedEvent) {
            // 这里可以根据 ctx.linesToClear.size() 播放不同音效
            PlayConsoleSound();
        }

        // 2. 捕捉游戏结束瞬间
        if (ctx.isGameOver && !m_playedGameOver) {
            // PlayGameOverSound();
            m_playedGameOver = true;
        }

        // 如果游戏重启了，重置标志位
        if (!ctx.isGameOver) m_playedGameOver = false;
    }

private:
    bool m_playedGameOver = false;
};
class GameEngine {
public:
    GameEngine(int targetFPS = 60) : m_ticker(targetFPS) {
        srand((unsigned int)time(NULL));
        m_logic.Reset(m_ctx);

    }
    void ProcessGameFrame(float dt) {
        // 1. 输入
        m_input.ProcessInput(m_ctx, m_logic);

        // 2. 逻辑更新
        if (!m_ctx.isPaused) {
            m_logic.Update(m_ctx, dt);
            m_ctx.ghostY = m_logic.CalculateLandY(m_ctx);
        }

        // 3. 表现与后处理
        m_audio.Update(m_ctx);
        PostUpdate();

        // 4. 渲染
        BeginDrawing();
        m_view.Render(m_ctx);
        EndDrawing();
    }
    void UpdateSystemState() {
        // 询问 View 窗口行不行
        m_ctx.isWindowInvalid = m_view.IsLayoutInvalid();

        if (m_ctx.isWindowInvalid) {
            if (!m_ctx.isPaused) {
                m_ctx.isPaused = true;
                m_ctx.isAutoPausedBySize = true; // 标记是系统干的
            }
        } else {
            // 窗口正常了，如果是系统刚才停的，就自动开
            if (m_ctx.isAutoPausedBySize) {
                m_ctx.isPaused = false;
                m_ctx.isAutoPausedBySize = false;
            }
        }
    }
    void Run() {
        while (!ConsoleWindowShouldClose() && !m_ctx.shouldExit) {
            float dt = m_ticker.Tick();

            // 1. 系统级检测（放在最前面，决定本帧走哪条路）
            UpdateSystemState();

            if (m_ctx.isWindowInvalid) {
                // 走“警告”渲染路径
                BeginDrawing();
                m_view.DrawSizeWarning();
                EndDrawing();
            } else {
                // 走“正常”游戏路径
                ProcessGameFrame(dt);
            }
        }
    }

private:
    void PostUpdate() {
        // 集中清理所有的瞬时状态（Flag）
        // 这样 main 函数就完全看不见这些琐事了
        m_ctx.lineClearedEvent = false;
        // 如果有其他每帧重置的状态，全写在这里
    }

    // 所有组件作为成员变量
    GameContext  m_ctx;
    GameLogic    m_logic;
    GameView     m_view;
    AudioManager m_audio;
    InputHandler m_input;
    TimeTicker   m_ticker;
};
int main() {

    SetTargetFPS(60);
    InitConsole(40, 30);

    GameEngine engine;
    engine.Run();
    return 0;

}