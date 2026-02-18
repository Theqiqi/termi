// GameContext.h - 这是你的 Model 数据中心
#pragma once
#include <vector>
struct GameContext {
    // --- 1. 静态环境 ---
    int board[20][10]; // 0:空, 1-7:颜色索引

    // --- 2. 动态实体 (Active Piece) ---
    int curPieceType;   // 当前形状 (I, J, L...)
    int curRotation;    // 0, 1, 2, 3
    int curX, curY;     // 位置

    // --- 3. 预测数据 ---
    int nextPieceType;  // 下一个方块

    // --- 4. 游戏状态 ---
    int score;
    int level;
    float fallTimer;    // 距离下次下落的累计时间
    bool isGameOver;
    bool isPaused;
    bool shouldExit=false;;
    float dropTimer;
    bool pendingLines[20];
    float dropInterval;
    bool lineClearedEvent = false; // 消行事件标志
    int ghostY;
    float lineClearTimer = 0.0f;     // 消行动效剩余时间
    std::vector<int> linesToClear;   // 记录哪几行需要闪烁
    // --- 新增：用于下落动画 ---
    float dropAnimTimer = 0.0f;     // 动画剩余时间
    int rowOffsets[20] = {0};       // 每一行需要视觉偏移的格数
    bool isWindowInvalid =false;
    bool isAutoPausedBySize;
    // 清理本帧的所有临时标志
    void ResetFrameEvents() {
        lineClearedEvent = false;
        // 其他标志...
    }
    //ai
    bool isAIMode;
    bool isWPressed;
    bool isBPressed;
    int rotation ;
    bool isDPressed ;
    bool isAPressed;
    bool isSpacePressed;
    bool curType;

};
