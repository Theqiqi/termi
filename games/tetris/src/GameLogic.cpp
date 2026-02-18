//
// Created by kali on 2/17/26.
//

#include "GameLogic.h"
#include <cstdlib>
#include "shp.h"
#include <algorithm>
#include <cmath>
#include "LogSystem.h"
GameLogic::GameLogic() {

}
GameLogic::~GameLogic() {

}
    /*
// 1. 处理即时输入（每帧检测，响应丝滑）
HandleInput(ctx);

// 2. 处理时间驱动的下落（受定时器控制）
ctx.fallTimer += GetFrameTime();
if (ctx.fallTimer >= ctx.fallSpeed) {
    ApplyGravity(ctx); // 执行向下移动一格的逻辑
    ctx.fallTimer = 0;
}
*/

void GameLogic::Update(GameContext& ctx, float dt) {

    if (ctx.isGameOver) return;
    // 更新下落动画计时器
    if (ctx.dropAnimTimer > 0) {
        ctx.dropAnimTimer -= dt;
    }
    if (ctx.isGameOver || ctx.isPaused) return; // 暂停拦截
    // A. 处理消行时的“时间冻结”动效
    if (HandleLineClearAnimation(ctx, dt)) return;

    // B. 处理正常重力下落逻辑
    ctx.dropTimer += dt;
    if (ctx.dropTimer >= ctx.dropInterval) {
        ProcessGravityStep(ctx);

        // 保留你原版的计时器溢出处理，精准！
        ctx.dropTimer -= ctx.dropInterval;
        if (ctx.dropTimer < 0) ctx.dropTimer = 0;
    }
    ctx.ghostY = CalculateLandY(ctx);
}


bool GameLogic::CheckCollision(const GameContext& ctx, int nextX, int nextY, int nextRot) {
    // 1. 现场拿数据，不再依赖 Context 里的缓存
    shp::Point points[4];
    shp::Get(ctx.curPieceType, nextRot, points);

    for (int i = 0; i < 4; i++) {
        int targetX = nextX + points[i].x;
        int targetY = nextY + points[i].y;

        // 判定 1：左右边界
        if (targetX < 0 || targetX >= 10) return true;

        // 判定 2：底部边界
        if (targetY >= 20) return true;

        // 判定 3：棋盘堆积块
        // (targetY < 0 时通常是方块还没掉进场，允许通过)
        if (targetY >= 0 && ctx.board[targetY][targetX] != 0) {
            return true;
        }
    }
    return false;
}

bool GameLogic::Move(GameContext& ctx, int dx, int dy) {
    if (ctx.lineClearTimer > 0) return false; // 🔒 消行锁定期间，不准动！
    int nextX = ctx.curX + dx;
    int nextY = ctx.curY + dy;

    // 先检查，如果没碰撞才修改坐标
    if (!CheckCollision(ctx, nextX, nextY, ctx.curRotation)) {
        ctx.curX = nextX;
        ctx.curY = nextY;
        return true;
    }

    return false; // 撞到了，移动失败
}

bool GameLogic::TryRotate(GameContext& ctx){
    int nextRot = (ctx.curRotation + 1) % 4; // 顺时针转

    // 检查转过去之后会不会撞墙或撞到别的方块
    if (!CheckCollision(ctx, ctx.curX, ctx.curY, nextRot)) {
        ctx.curRotation = nextRot;
        return true;
    }

    // 如果撞了，就原封不动
    return false;
}

void GameLogic::LockToBoard(GameContext& ctx) {
    shp::Point points[4];
    shp::Get(ctx.curPieceType, ctx.curRotation, points);

    for (int i = 0; i < 4; i++) {
        int tx = ctx.curX + points[i].x;
        int ty = ctx.curY + points[i].y;

        // 只有在屏幕范围内才写入（防止越界）
        if (tx >= 0 && tx < 10 && ty >= 0 && ty < 20) {
            // 将方块类型存入地图，比如 I 形存 1，O 形存 2...
            ctx.board[ty][tx] = ctx.curPieceType + 1;
        }
    }
}

int GameLogic::ClearLines(GameContext& ctx) {
    ctx.linesToClear.clear();
    for (int y = 0; y < 20; y++) {
        bool full = true;
        for (int x = 0; x < 10; x++) {
            if (ctx.board[y][x] == 0) { full = false; break; }
        }
        if (full) {
            ctx.linesToClear.push_back(y); // 只标记，不删除
        }
    }

    if (!ctx.linesToClear.empty()) {
        ctx.lineClearTimer = 0.2f;   // 开启计时
        ctx.lineClearedEvent = true; // 触发音效标志
        return ctx.linesToClear.size();
    }
    return 0;
}
void GameLogic::SpawnPiece(GameContext& ctx) {
    // 如果已经死了，直接返回，不要再次触发死亡逻辑
    if (ctx.isGameOver) return;

    ctx.curPieceType = ctx.nextPieceType;
    ctx.nextPieceType = rand() % 7;
    ctx.curRotation = 0;
    ctx.curX = 3;
    ctx.curY = 0;

    // 出生瞬间碰撞检查
    if (CheckCollision(ctx, ctx.curX, ctx.curY, ctx.curRotation)) {
        ctx.isGameOver = true;
        LogSystem::Log("!!! GAME OVER !!! Piece blocked at Spawn");
    }
}
int GameLogic::CalculateScore(int lines) {
    // 采用经典的俄罗斯方块计分倍率
    switch (lines) {
        case 1: return 100;
        case 2: return 300;
        case 3: return 700;
        case 4: return 800; // 奖励一次消四行（Tetris!）
        default: return 0;
    }
}

void GameLogic::Reset(GameContext& ctx) {
    // 1. 清空棋盘（最重要的！）
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 10; x++) ctx.board[y][x] = 0;
    }

    // 2. 状态全部归零
    ctx.isGameOver = false;
    ctx.isPaused = false;
    ctx.score = 0;
    ctx.lineClearTimer = 0;
    ctx.dropTimer = 0;
    ctx.dropInterval = 0.5f; // 重置速度

    // 3. 生成第一个方块
    ctx.nextPieceType = rand() % 7;
    SpawnPiece(ctx);

    LogSystem::Log("--- GAME RESET SUCCESS ---");
}
int GameLogic::CalculateLandY(const GameContext& ctx) {
    int landY = ctx.curY;

    // 模拟往下掉，直到撞到东西为止
    while (!CheckCollision(ctx, ctx.curX, landY + 1, ctx.curRotation)) {
        landY++;
    }
    return landY;
}

void GameLogic::HardDrop(GameContext& ctx) {
    if (ctx.isGameOver || ctx.lineClearTimer > 0) return; // 死亡拦截

    ctx.curY = CalculateLandY(ctx);
    LockToBoard(ctx);

    int lines = ClearLines(ctx);
    if (lines > 0) {
        ApplyScoreAndFeedback(ctx, lines);
    } else {
        SpawnPiece(ctx);
    }
}
void GameLogic::ExecutePhysicClear(GameContext& ctx) {
    if (ctx.linesToClear.empty()) return;

    // 1. 初始化偏移数组
    for (int i = 0; i < 20; i++) ctx.rowOffsets[i] = 0;

    // 2. 降序排列
    std::sort(ctx.linesToClear.begin(), ctx.linesToClear.end(), std::greater<int>());

    // 3. 计算偏移：对于每一个被消掉的行，它上方的所有行都要下落一格
    for (int rowToClear : ctx.linesToClear) {
        for (int y = 0; y < rowToClear; y++) {
            ctx.rowOffsets[y]++;
        }
    }

    // 4. 物理搬运（执行你之前那个稳健的 newBoard 搬运逻辑）
    int newBoard[20][10] = {0};
    int writeRow = 19;
    for (int y = 19; y >= 0; --y) {
        bool isCleared = false;
        for (int cRow : ctx.linesToClear) {
            if (y == cRow) { isCleared = true; break; }
        }
        if (!isCleared) {
            for (int x = 0; x < 10; ++x) newBoard[writeRow][x] = ctx.board[y][x];
            writeRow--;
        }
    }
    for (int y = 0; y < 20; ++y) {
        for (int x = 0; x < 10; ++x) ctx.board[y][x] = newBoard[y][x];
    }

    // 5. 开启下落动画计时（0.15秒足够丝滑）
    ctx.dropAnimTimer = 0.2f;
    ctx.linesToClear.clear();
}
bool GameLogic::HandleLineClearAnimation(GameContext& ctx, float dt) {
    if (ctx.lineClearTimer > 0) {
        ctx.lineClearTimer -= dt;
        if (ctx.lineClearTimer <= 0) {
            ExecutePhysicClear(ctx); // 物理消除
            SpawnPiece(ctx);         // 生成新方块 <-- 关键！AI 就等这一句
            ctx.lineClearTimer = 0;
        }
        return true;
    }
    return false;
}

void GameLogic::ProcessGravityStep(GameContext& ctx) {
    if (Move(ctx, 0, 1)) return;

    LockToBoard(ctx);
    int lines = ClearLines(ctx);

    if (lines > 0) {
        ApplyScoreAndFeedback(ctx, lines);
        // 注意：这里不再立即检测死亡，也不 SpawnPiece
        // 等 0.2s 动画结束后，在 HandleLineClearAnimation 里处理
    } else {
        SpawnPiece(ctx);
        // 没消行才立即检测死亡
        if (CheckCollision(ctx, ctx.curX, ctx.curY, ctx.curRotation)) {
            ctx.isGameOver = true;
        }
    }
}
void GameLogic::ApplyScoreAndFeedback(GameContext& ctx, int lines) {
    if (lines <= 0) return;

    // 1. 非线性计分
    int scores[] = {0, 100, 300, 500, 800};
    int linesIndex = (lines > 4) ? 4 : lines;
    ctx.score += scores[linesIndex];

    // 2. 触发状态机：设置消行闪烁倒计时
    // 只有设置了这个，Update 里的 HandleLineClearAnimation 才会生效
    ctx.lineClearTimer = 0.2f;

    // 3. 记录哪些行需要被消除（供动效渲染和后期物理消除使用）
    // 这里的 linesToClear 应该已经在你的 ClearLines 函数里 push_back 过了

    // 4. 音效与事件反馈
    ctx.lineClearedEvent = true;

    // 5. 难度随分数动态提升
    // 初始间隔 0.5s，每 1000 分加快 10%，但最快不能快过 0.1s
    float newInterval = 0.5f * pow(0.9f, (float)ctx.score / 1000.0f);
    ctx.dropInterval = (newInterval < 0.1f) ? 0.1f : newInterval;
}
// 增加 pieceType 参数，让 AI 能够指定方块种类进行模拟
bool GameLogic::IsPositionValid(int pieceType, int x, int y, int r, const int board[20][10]) {
    shp::Point pts[4];
    shp::Get(pieceType, r, pts);

    for (int i = 0; i < 4; i++) {
        int tx = x + pts[i].x;
        int ty = y + pts[i].y;

        // 1. 墙体检查
        if (tx < 0 || tx >= 10 || ty >= 20) return false;

        // 2. 只有当 ty >= 0 时才检查棋盘碰撞
        if (ty >= 0) {
            // 如果这里撞了，说明 board[0][x] 真的有东西
            if (board[ty][tx] != 0) return false;
        }
        // 注意：如果 ty < 0，应该直接 return true（允许方块在屏幕上方）
    }
    return true;
}
bool GameLogic::IsPositionValid(const GameContext& ctx, int x, int y, int r) {
    // 核心修正：必须从 ctx 中取出当前的 curPieceType 传给接口 A
    return IsPositionValid(ctx.curPieceType, x, y, r, ctx.board);
}
int GameLogic::SimulateDrop(const GameContext& ctx, int x, int r, int tempBoard[20][10]) {
    // 1. 检查起始位置（使用 ctx.curPieceType）
    if (!IsPositionValid(ctx.curPieceType, x, 0, r, tempBoard)) return -1;

    // 2. 模拟下落
    int y = 0;
    while (IsPositionValid(ctx.curPieceType, x, y + 1, r, tempBoard)) {
        y++;
    }

    // 3. 写入模拟棋盘
    shp::Point pts[4];
    shp::Get(ctx.curPieceType, r, pts);
    for (int i = 0; i < 4; i++) {
        int tx = x + pts[i].x;
        int ty = y + pts[i].y;
        if (tx >= 0 && tx < 10 && ty >= 0 && ty < 20) {
            tempBoard[ty][tx] = 1; // 标记占用
        }
    }
    return y;
}
