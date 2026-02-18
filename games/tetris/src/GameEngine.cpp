//
// Created by kali on 2/18/26.
//
#include <iostream>
#include "GameEngine.h"
#include "LogSystem.h"
GameEngine::GameEngine(int targetFPS)
    : m_ticker(targetFPS) {
    LogSystem::InitDebugFile(); // <--- 加上这句，每次运行先清空旧日志
    srand((unsigned int)time(NULL));
    m_logic.Reset(m_ctx);
}
void GameEngine::Run() {
    // 只要窗口没关，且玩家没点退出，就一直跑
    while (!ConsoleWindowShouldClose() && !m_ctx.shouldExit) {

        // 1. 每帧开始，先获取这一帧的时间步长
        float dt = m_ticker.Tick();

        // 2. 核心状态判定：窗口是否有效？
        // 这个函数依然保持在 Engine 顶层，因为它决定了执行哪条路径
        UpdateSystemState();

        if (m_ctx.isWindowInvalid) {
            // 路径 A: 窗口太小，强制暂停并只渲染警告
            BeginDrawing();
            m_view.DrawSizeWarning();
            EndDrawing();
        }
        else {
            // 路径 B: 正常路径，调用你刚才拆分好的流水线
            ProcessGameFrame(dt);
        }
    }
}
void GameEngine::UpdatePhysics(float dt) {
    if (m_ctx.isAIMode) {
        // AI 模式下，我们大幅增加 dt，或者直接调用 Tick
        // 让方块在 AI 思考完路径后立刻下落
        m_logic.Update(m_ctx, dt * 5.0f); // 5倍速，或者直接用下面更狠的方法
    } else {
        m_logic.Update(m_ctx, dt);
    }
    m_ctx.ghostY = m_logic.CalculateLandY(m_ctx);
}
void GameEngine::ProcessGameFrame(float dt) {
    GatherInput();

    // 1. 动画更新（独立于 AI 和 暂停之外，除非你希望暂停时动画也停）
    m_logic.HandleLineClearAnimation(m_ctx, dt);

    // 核心拦截：如果死了或暂停了，后面 AI 和物理统统不跑
    if (m_ctx.isPaused || m_ctx.isGameOver) {
        GenerateFeedback(); // 哪怕死了也要渲染最后一帧
        return;
    }

    if (m_ctx.isAIMode) {
        // 只有当消行动画结束（timer <= 0）且 没死 的时候才思考
        if (m_ctx.lineClearTimer <= 0) {
            static float aiTick = 0;
            aiTick += dt;
            // 建议将 AI 速度调慢一点点（0.2s），方便肉眼观察
            if (aiTick >= 0.2f) {
                m_ai.Think(m_ctx, m_logic);
                aiTick = 0;
            }
        }
    } else {
        if (m_ctx.lineClearTimer <= 0) {
            UpdatePhysics(dt);
        }
    }

    GenerateFeedback();
}

void GameEngine::GatherInput() {
    // 关键点 A：本帧唯一一次按键抓取
    int rawKey = GetKeyPressed();
    if (rawKey <= 0) return; // 没按键直接走，节省 CPU

    // --- 第一层：系统级绝对优先级 (不受任何 isPaused 或 isGameOver 影响) ---

    // 1. ESC 退出
    if (rawKey == 27) {
        m_ctx.shouldExit = true;
        LogSystem::Log("System: EXIT Requested");
        return;
    }

    // 2. R 重置 (必须在最前面，这样死了也能重开)
    if (rawKey == KEY_R || rawKey == 'r' || rawKey == 'R') {
        LogSystem::Log("System: RESET Requested");
        m_logic.Reset(m_ctx);
        return; // 重置后立刻结束，防止本帧按键影响新开的一局
    }

    // 3. P 暂停
    if (rawKey == KEY_P || rawKey == 'p' || rawKey == 'P') {
        m_ctx.isPaused = !m_ctx.isPaused;
        LogSystem::Log(m_ctx.isPaused ? "System: PAUSE ON" : "System: PAUSE OFF");
        return; // 暂停切换完也要 return，防止暂停瞬间方块还被操作
    }

    // 4. M 模式切换
    if (rawKey == KEY_M || rawKey == 'm' || rawKey == 'M') {
        m_ctx.isAIMode = !m_ctx.isAIMode;
        LogSystem::Log(m_ctx.isAIMode ? "System: AI MODE ON" : "System: AI MODE OFF");
        return;
    }

    // --- 第二层：逻辑拦截 (暂停或死亡时，严禁操作方块) ---
    if (m_ctx.isPaused || m_ctx.isGameOver || m_ctx.lineClearTimer > 0) {
        return;
    }

    // --- 第三层：游戏内控制 (仅手动模式) ---
    if (!m_ctx.isAIMode) {
        // 这里只处理上下左右和空格
        // 注意：不要再让 ProcessGameKeys 内部去调用 GetKeyPressed() 了！
        m_input.ProcessGameKeys(m_ctx, m_logic, rawKey);
    }
}

void GameEngine::GenerateFeedback() {
    m_audio.Update(m_ctx); // 播放音效
    PostUpdate();          // 清理每帧状态 (如 lineClearedEvent)

    BeginDrawing();
    m_view.Render(m_ctx);  // 渲染画面
    EndDrawing();
}

void GameEngine::PostUpdate() {
    // 集中清理所有的瞬时状态（Flag）
    // 这样 main 函数就完全看不见这些琐事了
    m_ctx.lineClearedEvent = false;
    // 如果有其他每帧重置的状态，全写在这里
}
void GameEngine::UpdateSystemState() {
    // 1. 调用 View 层检测当前窗口布局是否依然有效 (比如是否被拉得太小)
    m_ctx.isWindowInvalid = m_view.IsLayoutInvalid();

    // 2. 自动暂停逻辑
    if (m_ctx.isWindowInvalid) {
        // 如果窗口无效且尚未暂停，强制进入“自动暂停”状态
        if (!m_ctx.isPaused) {
            m_ctx.isPaused = true;
            m_ctx.isAutoPausedBySize = true; // 标记是因为窗口原因才停的
        }
    } else {
        // 窗口恢复正常，如果之前是因为窗口小才停的，现在自动恢复
        if (m_ctx.isAutoPausedBySize) {
            m_ctx.isPaused = false;
            m_ctx.isAutoPausedBySize = false;
        }
    }
}
