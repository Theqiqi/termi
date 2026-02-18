#pragma once

#include "GameLogic.h"

#include <stdio.h>
#include <assert.h>
#include "tested.h"


void Test_Basic_Movement() {
    // 1. 准备：一个完全干净的棋盘
    GameContext ctx;
    GameLogic logic;

    // 初始化方块在中心 (5, 5)
    ctx.curX = 5;
    ctx.curY = 5;
    ctx.curPieceType = 0; // I形
    ctx.curRotation = 0;
    // 确保棋盘是空的
    for(int y=0; y<20; y++)
        for(int x=0; x<10; x++) ctx.board[y][x] = 0;

    // 2. 执行：向右移动一格
    bool result = logic.Move(ctx, 1, 0);

    // 3. 断言 (Verification)
    if (result == true && ctx.curX == 6 && ctx.curY == 5) {
        printf("[PASS] 基础移动测试：方块从 (5,5) 成功移动到 (6,5)\n");
    } else {
        printf("[FAIL] 基础移动测试：坐标未按预期更新！当前坐标: (%d, %d)\n", ctx.curX, ctx.curY);
    }
}
void Test_True_Wall_Collision() {
    GameContext ctx;
    GameLogic logic;
    ctx.curPieceType = 1; // O形 (x定义为 1, 2)

    // 1. 此时盒子在 -1，方块最左点在 0 (-1+1)，紧贴左墙
    ctx.curX = -1;

    // 2. 尝试再往左移到 -2
    bool result = logic.Move(ctx, -1, 0);

    // 3. 计算：nextX(-2) + point.x(1) = -1
    // 此时 targetX < 0 成立！

    if (result == false) {
        printf("拦截成功！逻辑没问题，方块确实不能再往左了。\n");
    }
}
void Test_LockToBoard_Logic() {
    GameContext ctx;
    GameLogic logic;

    // 1. 初始化环境：一个干净的棋盘
    for(int y=0; y<20; y++)
        for(int x=0; x<10; x++) ctx.board[y][x] = 0;

    // 2. 准备一个 O形方块 (假设类型为 1，坐标是 {1,0}, {2,0}, {1,1}, {2,1})
    ctx.curPieceType = 1;
    ctx.curRotation = 0;

    // 3. 把方块放在地板上方：Y=18
    // 此时方块的点在 Y=18 和 Y=19 (18+0, 18+1)
    // 如果再往下移 (Y=19)，方块的点就会达到 Y=20，触发撞底
    ctx.curX = 0;
    ctx.curY = 18;

    // 4. 执行下落动作
    bool canMoveDown = logic.Move(ctx, 0, 1);

    // 5. 逻辑判定：如果不能下落，则执行固化
    if (canMoveDown == false) {
        printf("[Step 1] 碰撞引擎正确拦截了触地动作。\n");

        logic.LockToBoard(ctx); // 执行你刚写好的函数

        // 6. 核心断言：检查棋盘上对应的点是否被修改
        // 根据 O形的定义，(0+1, 18+0) -> (1, 18) 应该是方块的颜色编号
        if (ctx.board[18][1] != 0 && ctx.board[19][2] != 0) {
            printf("[Step 2] LockToBoard 成功！方块已固化到 board 数组。\n");
        } else {
            printf("[FAIL] LockToBoard 失败：棋盘数组对应的位置还是空的！\n");
        }
    } else {
        printf("[FAIL] 逻辑错误：方块竟然穿过了地板！\n");
    }
}
void Test_Time_Freezing_During_Animation() {
    GameContext ctx;
    GameLogic logic;
    ctx.lineClearTimer = 0.2f; // 模拟正在消行
    ctx.dropTimer = 0.1f;
    ctx.dropInterval = 0.5f;

    logic.Update(ctx, 0.1f);

    // 验证：正在消行时，重力计时器 dropTimer 不应该增加
    assert(ctx.dropTimer == 0.1f);
    assert(ctx.lineClearTimer < 0.2f);
    printf("[PASS] 时间冻结测试：消行期间重力已挂起。\n");
}
void Test_Spawn_New_Piece_V2() {
    GameContext ctx{};
    GameLogic logic;

    // 1. 模拟方块已经到底部，且下方是空的物理边界（20行）
    ctx.curPieceType = 1;
    ctx.curX = 4;
    while(logic.Move(ctx, 0, 1)) { ctx.curY++; }

    // 2. 确保不会触发消行 (清空底行)
    for(int x=0; x<10; x++) ctx.board[19][x] = 0;

    // 3. 触发 Update
    ctx.dropInterval = 0.5f;
    ctx.dropTimer = 0.5f;
    logic.Update(ctx, 0.01f);

    // 4. 验证：由于没消行，不走 timer 逻辑，直接重生
    // 重生后 curY 应该回到 0（或者你定义的起始 Y）
    assert(ctx.curY == 0);
    assert(ctx.lineClearTimer == 0);
    printf("[PASS] Test_Spawn_New_Piece_V2\n");
}
void Test_Logic_Rules_V2() {
    GameLogic logic;

    // --- 场景 1：左边界碰撞 ---
    {
        GameContext ctx{};
        ctx.curPieceType = 1;
        ctx.curX = 5; // 先放在中间

        // 一直往左移，直到撞墙
        while(logic.Move(ctx, -1, 0)) { ctx.curX--; }

        // 此时已经在最左了，尝试再往左移动 -1
        bool canMoveLeftAgain = logic.Move(ctx, -1, 0);
        assert(canMoveLeftAgain == false);
        printf("[PASS] Left wall collision detection.\n");
    }

    // --- 场景 2：下方障碍物碰撞 ---
    {
        GameContext ctx{};
        ctx.curPieceType = 1;
        ctx.curX = 5;
        ctx.curY = 5;

        // 在方块的正下方远处放个障碍
        ctx.board[15][5] = 1;
        ctx.board[15][6] = 1; // 考虑到方块宽度，多放几个点确保撞上

        // 自动下落直到撞到障碍物
        while(logic.Move(ctx, 0, 1)) { ctx.curY++; }

        // 断言：此时 curY 应该在障碍物上方，不能再往下
        assert(logic.Move(ctx, 0, 1) == false);
        printf("[PASS] Bottom obstacle collision detection.\n");
    }
}
void Test_The_Ultimate_Cycle_V2() {
    GameContext ctx{};
    GameLogic logic;

    // 1. 初始化：手动构造一个“准满行”
    for (int x = 0; x < 10; x++) ctx.board[19][x] = 1;
    ctx.board[19][5] = 0; // 5号位有个洞

    // 2. 模拟方块正好落入并填满了这个洞
    // 我们假设逻辑已经运行到了这里，手动把洞堵上
    ctx.board[19][5] = 2; // 用不同的编号模拟新落下的方块

    // 3. 此时手动触发消行扫描
    // 这是为了验证 ClearLines 能否正确识别满行并设置 Timer
    int lines = logic.ClearLines(ctx);

    // --- 关键断言 1：扫描必须成功 ---
    if (lines != 1 || ctx.lineClearTimer <= 0) {
        printf("[FAIL] ClearLines 未能识别满行或未设置计时器。Lines: %d, Timer: %.2f\n",
                lines, ctx.lineClearTimer);
        assert(false);
    }

    // 4. 模拟 Update 推动状态机
    // 此时 timer = 0.2，传入 dt=0.01，逻辑应该处于“倒计时中”，不执行搬运
    logic.Update(ctx, 0.01f);
    assert(ctx.lineClearTimer > 0);
    assert(ctx.board[19][5] != 0); // 动效期间，方块像素必须还在

    // 5. 模拟时间流逝，跨越 0.2s 阈值
    // 此时 HandleLineClearAnimation 应该触发 ExecutePhysicClear
    logic.Update(ctx, 0.3f);

    // --- 关键断言 2：物理消除结果 ---
    assert(ctx.lineClearTimer <= 0); // 计时器归零
    if (ctx.board[19][5] != 0) {
        printf("[FAIL] 物理消除未执行，第 19 行依然残留数据！\n");
        assert(false);
    }

    // 6. 验证重生：因为 Update 内部在动画结束时会调用 SpawnPiece
    // 重生后的坐标通常是 curY = 0
    assert(ctx.curY == 0);

    printf("[PASS] Test_The_Ultimate_Cycle_V2 成功通过 (手动模拟固化路径)！\n");
}
void Test_CalculateLandY() {
    GameContext ctx{};
    GameLogic logic;

    ctx.curPieceType = 1; // O型 (本地y占 0, 1)
    ctx.curX = 4;         // 配合 O型的本地x(1,2)，实际占据棋盘 x=5,6
    ctx.curY = 0;

    // 场景：下方有障碍物
    ctx.board[10][5] = 1;
    int landY = logic.CalculateLandY(ctx);

    // 修正：方块占据 landY(8) 和 landY+1(9)，下方 10 是障碍
    // 所以 landY 应该是 8
    assert(landY == 8);

    printf("[PASS] CalculateLandY 深度探测准确。结果为: %d\n", landY);
}
void Test_Ghost_Piece_Logic_Fixed() {
    GameContext ctx{};
    GameLogic logic;

    // 1. 设置 I 型横放 (Type 0, Rot 0)
    ctx.curPieceType = 0;
    ctx.curRotation = 0;
    ctx.curX = 5;
    ctx.curY = 0;

    // 2. 在物理坐标 (5, 10) 放障碍
    // 横着的 I 型点的本地 y 偏移是 1
    for(int y=0; y<20; y++) for(int x=0; x<10; x++) ctx.board[y][x] = 0;
    ctx.board[10][5] = 1;

    int landY = logic.CalculateLandY(ctx);

    // 3. 精确推理：
    // 当 landY = 8，物理 y = 8 + 1 = 9 (OK)
    // 当 landY = 9，物理 y = 9 + 1 = 10 (撞击!)
    // 所以结果必须是 8
    if (landY != 8) {
        printf("[FAIL] CalculateLandY 预期 8，实际得到 %d\n", landY);
        assert(landY == 8);
    }

    printf("[PASS] I型方块投影计算正确，停在 landY = 8 (物理 y=9)\n");
}
void Test_Rotation_Safety() {
    GameContext ctx{};
    GameLogic logic;

    // --- 场景：I型长条贴左墙旋转 ---
    // 1. 初始化为 I型 (Type 0)，竖放 (Rot 1)
    // 根据你的 shp::DATA，竖放的 I 型本地 x 偏移全是 2
    ctx.curPieceType = 0;
    ctx.curRotation = 1; // 竖着
    ctx.curX = -2;       // 此时物理坐标 x = -2 + 2 = 0，紧贴左墙

    // 2. 尝试旋转回横向 (Rot 0)
    // 横向的 I 型点本地 x 偏移是 0, 1, 2, 3
    // 旋转后物理坐标将变成 -2, -1, 0, 1 -> 明显越界 ( < 0)
    bool rotated = logic.TryRotate(ctx);

    // 3. 断言：应该旋转失败，且状态不被修改
    assert(rotated == false);
    assert(ctx.curRotation == 1);

    printf("[PASS] 旋转安全测试：成功拦截了左墙越界旋转。\n");
}
void Test_Game_Over_Condition() {
    GameContext ctx{};
    GameLogic logic;

    // 1. 将出生点区域（顶部两行）全部填满
    // 这样不管 SpawnPiece 随机出什么形状，都一定会发生碰撞
    for (int x = 0; x < 10; x++) {
        ctx.board[0][x] = 1;
        ctx.board[1][x] = 1;
    }

    // 2. 执行重生
    logic.SpawnPiece(ctx);

    // 3. 断言
    if (ctx.isGameOver) {
        printf("[PASS] 死亡判定测试成功：顶层阻塞导致游戏结束。\n");
    } else {
        printf("[FAIL] 死亡判定失败：棋盘顶层已满，但游戏未结束！\n");
        assert(ctx.isGameOver == true);
    }
}
void Test_Complex_Rotation_Obstacle() {
    GameContext ctx{};
    GameLogic logic;

    // 1. 构造一个“陷阱”：在左右两边放上固化方块，中间留两格
    // 假设 T 型方块 (Type 2) 旋转时需要三格空间
    ctx.board[5][3] = 1;
    ctx.board[5][5] = 1;

    // 2. 放置一个 T 型方块在中间 (y=5, x=4)
    ctx.curPieceType = 2;
    ctx.curX = 3; // 结合 T型本地坐标 x 偏移 (0,1,2)，curX=3 占据 3,4,5
    ctx.curY = 4;
    ctx.curRotation = 0; // T型横放

    // 3. 尝试旋转
    // 如果旋转后的点会撞到 (5,3) 或 (5,5)，旋转应该被拦截
    bool rotated = logic.TryRotate(ctx);

    assert(rotated == false);
    printf("[PASS] 复杂环境碰撞测试：成功拦截了拥挤环境下的旋转。\n");
}

void Tested() {
    Test_Basic_Movement();
    Test_True_Wall_Collision();
    Test_LockToBoard_Logic();
    Test_Time_Freezing_During_Animation();
    Test_Logic_Rules_V2();
    Test_Spawn_New_Piece_V2();
    Test_The_Ultimate_Cycle_V2();
    Test_CalculateLandY();
    Test_Ghost_Piece_Logic_Fixed();
    Test_Rotation_Safety();
    Test_Game_Over_Condition();
    Test_Complex_Rotation_Obstacle();

}