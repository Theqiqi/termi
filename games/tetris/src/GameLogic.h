// GameLogic.h
#pragma once
#include "GameContext.h"

class GameLogic {
public:
    GameLogic();
    ~GameLogic();

    /*
    // --- 玩家行为接口 ---
    void MoveLeft(GameContext& ctx);
    void MoveRight(GameContext& ctx);
    void Rotate(GameContext& ctx);
    void SoftDrop(GameContext& ctx); // 手动向下移一格
*/
    // 1. 系统控制接口
    // 初始化或重置游戏状态（清空棋盘、重置分数、生成首个方块）
    void Reset(GameContext& ctx);

    // 2. 核心驱动接口 (由 main 函数每一帧调用)
    // 每一帧的定时更新（重力、锁定、消行）
    // dt: 两帧之间的时间间隔，用于累计 fallTimer
    void Update(GameContext& ctx, float dt);
    int CalculateScore(int lines);
    // 3. 玩家交互接口 (返回 bool 表示操作是否成功)
    // 移动：传入 dx, dy。左(-1,0), 右(1,0), 下(0,1)
    // 返回 bool: 告诉外部这次移动是否成功（用于播放音效或停止动作）
    bool Move(GameContext& ctx, int dx, int dy);
    // 旋转：1为顺时针，-1为逆时针
    // 旋转：通常顺时针旋转
    bool TryRotate(GameContext& ctx);
    // 硬降：直接坠落到底部
    void HardDrop(GameContext& ctx);
    //ai
    int SimulateDrop(const GameContext& ctx, int x, int r, int tempBoard[20][10]);
    bool IsPositionValid(int x, int y, int r, const int board[20][10]);
    bool IsPositionValid(const GameContext& ctx, int x, int y, int r) {
        return IsPositionValid(x, y, r, ctx.board);
    }
//private:
public :

    // 生成新方块
    // 生成新方块：随机设置 ctx.activePiece，并检查是否一生成就碰撞（Game Over）
    void SpawnPiece(GameContext& ctx);
    int CalculateLandY(const GameContext& ctx);
    // --- 内部核心校验逻辑 (单元测试的重点) ---
    // 碰撞检查：检查方块在特定位置和旋转状态下是否合法
    // 这是整个游戏逻辑的灵魂，TryMove 和 TryRotate 都要调用它
    // --- 物理检测接口 (单元测试的核心) ---
    // 综合碰撞检测：传入想要尝试的位置和角度，判断是否合法
    // 它合并了你说的“墙壁检测”和“底座检测”
    bool CheckCollision(const GameContext& ctx, int nextX, int nextY, int nextRot);
    // 满行检查与消除：扫描 board，返回消除的行数，并更新 ctx.score
    int ClearLines(GameContext& ctx);

    // --- 核心物理规则 (不暴露给外部，只给内部函数用) ---
    // 动作：将活跃方块固定到 board 数组里
    // 方块固化：当方块不能再下落时，将其存入 ctx.board
    // 当 CheckCollision 发现下方有阻碍时调用
    void LockToBoard(GameContext& ctx);
    void ExecutePhysicClear(GameContext& ctx);
    bool HandleLineClearAnimation(GameContext& ctx, float dt);
    void ProcessGravityStep(GameContext& ctx);
    void ApplyScoreAndFeedback(GameContext& ctx, int lines);

};

