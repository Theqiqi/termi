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
    m_ctx.aiSpeedMode = AI_SMOOTH;
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
    // 即使在 AI 模式下，如果 lineClearTimer 刚结束，可能需要这一步来刷新棋盘
    // 建议把“行下移”的逻辑从 UpdatePhysics 剥离，放在一个独立的 m_logic.UpdateBoard() 里
    m_logic.Update(m_ctx, dt);
}
// GameEngine.cpp 里的 ProcessGameFrame 函数
void GameEngine::ProcessGameFrame(float dt) {
    GatherInput();

    // 1. 优先级最高：消行、暂停、结束（物理层拦截）
    if (m_ctx.isPaused || m_ctx.isGameOver || m_ctx.lineClearTimer > 0) {
        m_logic.Update(m_ctx, dt); // 确保消行下落动画运行
        GenerateFeedback();
        return;
    }

    // 2. 逻辑分发
    if (m_ctx.isAIMode) {
        HandleAILogic(dt); // 抽离出来的 AI 专用函数
    } else {
        UpdatePhysics(dt); // 手动模式
    }

    m_ctx.ghostY = m_logic.CalculateLandY(m_ctx);
    GenerateFeedback();
}
void GameEngine::ExecuteAIDecision() {
    if (!m_ctx.hasAIDecision) return;

    // 快速模式逻辑
    if (m_ctx.aiSpeedMode == AI_INSTANT) {
        m_ctx.curX = m_ctx.cachedTargetX;
        m_ctx.curRotation = m_ctx.cachedTargetRotation;
        m_logic.HardDrop(m_ctx);
        m_ctx.hasAIDecision = false; // 必须设为 false
        return;
    }

    // 慢速模式逻辑
    bool changed = false;
    if (m_ctx.curRotation != m_ctx.cachedTargetRotation) {
        if (!m_logic.TryRotate(m_ctx)) {
            m_ctx.hasAIDecision = false; // 转不动就重新想
            return;
        }
        changed = true;
    }
    else if (m_ctx.curX != m_ctx.cachedTargetX) {
        int dir = (m_ctx.cachedTargetX > m_ctx.curX) ? 1 : -1;
        if (!m_logic.Move(m_ctx, dir, 0)) {
            m_ctx.hasAIDecision = false; // 走不动就重新想
            return;
        }
        changed = true;
    }

    if (!changed) {
        // 已经对准，尝试下落
        if (!m_logic.Move(m_ctx, 0, 1)) {
            m_logic.ProcessGravityStep(m_ctx);
            m_ctx.hasAIDecision = false;
        }
    }
}
// GameEngine.cpp

void GameEngine::HandleAILogic(float dt) {
    m_ctx.aiTimer += dt;
    float delay = (m_ctx.aiSpeedMode == AI_INSTANT) ? 0.01f : 0.15f;

    // 1. 只有当 pieceID 改变，或者当前没有有效决策时，才 Think
    if (!m_ctx.hasAIDecision || m_ctx.lastThinkPieceID != m_ctx.pieceID) {
        AIDecision dec = m_ai.Think(m_ctx, m_logic);
        if (dec.isValid) {
            m_ctx.cachedTargetX = dec.targetX;
            m_ctx.cachedTargetRotation = dec.targetRotation;
            m_ctx.hasAIDecision = true;
            m_ctx.lastThinkPieceID = m_ctx.pieceID;
        } else {
            // 如果 AI 完全找不到路，强制让方块下落一格，防止死等卡死
            if (m_ctx.aiTimer >= delay) {
                m_logic.Move(m_ctx, 0, 1);
                m_ctx.aiTimer = 0;
            }
            return;
        }
    }

    // 2. 执行决策
    if (m_ctx.aiTimer >= delay) {
        ExecuteAIDecision();
        m_ctx.aiTimer = 0;
    }
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


    if (rawKey == KEY_N || rawKey == 'n' || rawKey == 'N') {
        if (m_ctx.aiSpeedMode == AI_INSTANT) {
            m_ctx.aiSpeedMode = AI_SMOOTH;
            LogSystem::Log("AI Speed: SMOOTH");
        } else {
            m_ctx.aiSpeedMode = AI_INSTANT;
            LogSystem::Log("AI Speed: INSTANT");
        }
        // 【关键】切换速度时，重置计时器，防止瞬间爆发
        m_ctx.aiTimer = 0;
        return;
    }
    // 4. M 模式切换
    if (rawKey == KEY_M || rawKey == 'm' || rawKey == 'M') {
        m_ctx.isAIMode = !m_ctx.isAIMode;
        // 【关键】切换模式时，重置决策状态，防止拿旧的决策去跑新模式
        m_ctx.hasAIDecision = false;
        m_ctx.lastThinkPieceID = -1; // 强制重新思考
        m_ctx.aiTimer = 0;
        LogSystem::Log(m_ctx.isAIMode ? "AI MODE ON" : "AI MODE OFF");
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
