//
// Created by kali on 2/18/26.
//
#include <iostream>
#include "GameEngine.h"

GameEngine::GameEngine(int targetFPS)
    : m_ticker(targetFPS) {
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
void GameEngine::UpdateSystemState() {
    // 1. 调用 View 层的接口检测窗口是否“爆框”
    m_ctx.isWindowInvalid = m_view.IsLayoutInvalid();

    // 2. 逻辑分发：处理自动暂停与恢复
    if (m_ctx.isWindowInvalid) {
        // 窗口太小，如果还没暂停，就强制开启“系统暂停”
        if (!m_ctx.isPaused) {
            m_ctx.isPaused = true;
            m_ctx.isAutoPausedBySize = true;
        }
    } else {
        // 窗口恢复正常了，检查是不是刚才因为窗口小才停的
        if (m_ctx.isAutoPausedBySize) {
            m_ctx.isPaused = false;         // 自动继续游戏
            m_ctx.isAutoPausedBySize = false; // 清除标记
        }
    }
}
void GameEngine::ProcessGameFrame(float dt) {
    // 第一步：感知与决策 (GatherInput)
    GatherInput();

    // 第二步：执行与物理 (UpdatePhysics)
    // 只有在没暂停且没有正在播放消行动画时才更新逻辑
    if (!m_ctx.isPaused && m_ctx.lineClearTimer <= 0) {
        UpdatePhysics(dt);
    }

    // 第三步：反馈与表现 (GenerateFeedback)
    GenerateFeedback();
}
void GameEngine::GatherInput() {
    // 全帧唯一的抓取点
    int rawKey = GetKeyPressed();

    // 1. 系统逻辑处理 (ESC, P, M, R)
    // 无论是否暂停，系统键都要响应
    m_input.ProcessSystemKeys(m_ctx, rawKey);

    // 2. 状态拦截判断
    // 如果窗口无效，或者已经暂停，或者正在消行动画，则不进行游戏逻辑决策
    if (m_ctx.isWindowInvalid || m_ctx.isPaused || m_ctx.lineClearTimer > 0) {
        return;
    }

    // 3. 决策者分发
    if (m_ctx.isAIMode) {
        // AI 决策时不依赖键盘 key，直接分析 ctx.board
        m_ai.Think(m_ctx, m_logic);
    } else {
        // 手动模式，传入刚才抓到的 rawKey
        m_input.ProcessGameKeys(m_ctx, m_logic, rawKey);
    }
}

void GameEngine::UpdatePhysics(float dt) {
    // 处理自动下落计时器
    m_logic.Update(m_ctx, dt);

    // 更新投影位置（Ghost Y）
    m_ctx.ghostY = m_logic.CalculateLandY(m_ctx);
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
