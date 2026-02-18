#include "GameView.h"
#include "ray_console.h" // 你的封装库
#include "shp.h"    // 包含 PIECE_DATA 表
#include <cstdio>
#include <cstring>
#include <cstdlib>

// 构造函数即 Init
GameView::GameView() {

        //SetConsoleTitle("Professional Tetris");
        //HideCursor();
        // 如果有 MapLogicToScreen 需要的偏移量，也可以在这里初始化
        //MapLogicToScreen(int lx, int ly, int& sx, int& sy);
}

// 析构函数即 Close
GameView::~GameView() {
        //ShowCursor();
        CloseConsole();
        printf("Resources cleaned up automatically.\n");
}


void GameView::Render(const GameContext& ctx){

        // 1. 正常绘制流程
        UpdateLayout(GetScreenWidth(), GetScreenHeight());
        ClearBackground();
        // 2. 苹果式的“细节强迫症”：强制刷新边界
        int frameWidth = 40;
        int startX = BOARD_OFFSET_X - 2;
        int startY = BOARD_OFFSET_Y - 1;
        int frameHeight = 22;
        int sepX = BOARD_OFFSET_X + 20;
        // 在 Render 函数中 DrawInfoPanel(ctx) 之前加入：
        // 矩形清理：清理信息区内部 (中隔墙右侧到右外墙左侧)
        // 确保这个循环擦干净了整个面板
        for (int iy = 0; iy < 21; iy++) {
                // 这里的 ix 范围要确保从隔墙到右侧外墙全部涂黑
                for (int ix = sepX + 1; ix < startX + frameWidth - 1; ix++) {
                        DrawPoint(ix, startY + 1 + iy, CG_COLOR_WHITE, ' ');
                }
        }
        DrawGameContent(ctx);
        DrawGameFrame();

        DrawInfoPanel(ctx);

        // 重新焊死中隔线和右外墙，确保任何文字溢出都被盖住
        DrawVLine(sepX, startY, frameHeight + 1, '#');
        DrawVLine(startX + frameWidth - 1, startY, frameHeight + 1, '#');

        // 再次加固四个关键角点
        DrawPoint(sepX, startY + frameHeight, CG_COLOR_BLUE, '#');
        DrawPoint(startX + frameWidth - 1, startY + frameHeight, CG_COLOR_BLUE, '#');

}
// --- 布局组件 ---
void GameView::DrawGameLayout() {
        DrawGameFrame();
        // 1. 绘制游戏区外框 (20行10列，映射到字符坐标是 20x22 左右)
        // 假设起始偏移量 OFFSET_X = 2, OFFSET_Y = 1
        // 2. 绘制信息区外框 (显示得分、下一个方块等)

}

// --- 游戏区核心 ---
void GameView::DrawGameContent(const GameContext& ctx) {
        // 1. 底层：背景与围墙（最先画，作为底色）
        DrawGameWall();

        // 2. 静态层：已经固化的方块点
        DrawStackedBlocks(ctx);

        // 3. 辅助层：阴影/虚影（放在活动方块之下，避免干扰视觉）
        DrawGhost(ctx);

        // 4. 动态层：正在下落的活跃方块（核心焦点）
        DrawActiveTetromino(ctx);

        // 5. 顶层：特效（消行闪烁、粒子等，需要覆盖在方块之上）
        DrawEffects(ctx);
}
// --- 信息区核心 ---
void GameView::DrawInfoPanel(const GameContext& ctx) {
        int startX = BOARD_OFFSET_X - 2;
        int frameWidth = 40;
        int rightWallX = startX + frameWidth - 1;
        int sepX = BOARD_OFFSET_X + 20;
        int centerX = (sepX + rightWallX) / 2;

        // --- 1. SCORE ---
        DrawTextCentered(centerX, BOARD_OFFSET_Y + 1, "SCORE", CG_COLOR_CYAN);
        char buf[16];
        sprintf(buf, "%06d", ctx.score);
        DrawTextEx(centerX - 3, BOARD_OFFSET_Y + 2, buf, CG_COLOR_WHITE);

        // --- 2. NEXT ---
        DrawTextCentered(centerX, BOARD_OFFSET_Y + 4, "NEXT", CG_COLOR_CYAN);
        DrawNextPieceCentered(centerX, BOARD_OFFSET_Y + 6, ctx.nextPieceType);

        // --- 3. STATUS & MODE ---
        DrawTextCentered(centerX, BOARD_OFFSET_Y + 10, "STATUS", CG_COLOR_CYAN);
        const char* st = ctx.isGameOver ? "GAME OVER" : (ctx.isPaused ? "PAUSED" : (ctx.isAIMode ? "AI RUNNING" : "MANUAL"));
        const char* stCol = ctx.isGameOver ? CG_COLOR_RED : (ctx.isPaused ? CG_COLOR_YELLOW : (ctx.isAIMode ? CG_COLOR_MAGENTA : CG_COLOR_GREEN));
        DrawTextCentered(centerX, BOARD_OFFSET_Y + 11, st, stCol);

        // 如果是 AI 模式，显示当前速度档位
        if (ctx.isAIMode) {
                const char* speedStr = (ctx.aiSpeedMode == 1) ? "MODE: INSTANT" : "MODE: NORMAL";
                DrawTextCentered(centerX, BOARD_OFFSET_Y + 12, speedStr, CG_COLOR_GRAY);
        }

        // --- 4. CONTROLS (动态切换) ---
        DrawControlsCentered(centerX, BOARD_OFFSET_Y + 15, ctx);
}


// 记得在 GameView.h 中把参数改成 (int centerX, int y, const GameContext& ctx)
void GameView::DrawControlsCentered(int centerX, int y, const GameContext& ctx)  {
        DrawTextCentered(centerX, y, "CONTROLS", CG_COLOR_CYAN);

        int sepX = BOARD_OFFSET_X + 20;
        int listX = centerX - 8;
        if (listX <= sepX) listX = sepX + 2;

        const char* c = CG_COLOR_YELLOW;

        if (ctx.isAIMode) {
                // AI 模式下的快捷键
                DrawTextEx(listX, y + 1, "M      : MANUAL", c);
                DrawTextEx(listX, y + 2, "1      : NORMAL", c);
                DrawTextEx(listX, y + 3, "2      : INSTANT", c);
                DrawTextEx(listX, y + 4, "R      : RESET",  c);
                DrawTextEx(listX, y + 5, "P      : PAUSE",  c);
        } else {
                // 手动模式下的快捷键
                DrawTextEx(listX, y + 1, "W      : ROTATE", c);
                DrawTextEx(listX, y + 2, "A,D    : MOVE",   c);
                DrawTextEx(listX, y + 3, "SPACE  : HARD",   c);
                DrawTextEx(listX, y + 4, "M      : AI MODE",c);
                DrawTextEx(listX, y + 5, "P      : PAUSE",  c);
        }
}
void GameView::DrawControlsPanel(int centerX, int y) {
        const char* kColor = CG_COLOR_YELLOW;

        // 标题居中
        DrawTextCentered(centerX, y, "CONTROLS", CG_COLOR_CYAN);

        // 列表项：我们手动计算一个起始点，让这组文字整体看起来在中间
        // 比如最长的一行 "SPACE  Hard " 大约 11 个字符，我们左移 5 个
        int listX = centerX - 6;

        DrawTextEx(listX, y + 1, "W      Rotate", kColor);
        DrawTextEx(listX, y + 2, "A,D    Move  ", kColor);
        DrawTextEx(listX, y + 3, "S      Drop  ", kColor);
        DrawTextEx(listX, y + 4, "SPACE  Hard  ", kColor);
        DrawTextEx(listX, y + 5, "P      Pause ", kColor);
}
void GameView::DrawNextPieceCentered(int centerX, int y, int type) {
        int t = type % 7;

        // 1. 先扫描该形状的逻辑宽度 (xmin, xmax)
        int minL = 4, maxL = 0;
        for (int i = 0; i < 4; i++) {
                int lx = shp::DATA[t][0][i][0];
                if (lx < minL) minL = lx;
                if (lx > maxL) maxL = lx;
        }

        // 2. 计算形状的逻辑宽度（例如 I 型是 3, O 型是 1, T 型是 2）
        // 注意：这里的宽度是逻辑坐标差
        int logicalWidth = maxL - minL;

        // 3. 计算物理宽度（逻辑宽度 * 2 + 2 字符）并找到物理中心点
        // 物理中心偏移量 = (总物理宽度 / 2)
        // 这里我们直接计算左起始点 sx：
        // centerX 是右面板的中轴，sx 应该是 centerX 减去 形状物理中心的一半
        int totalPhysicalWidth = (logicalWidth * 2) + 2;
        int visualOffsetX = centerX - (totalPhysicalWidth / 2);

        // 4. 渲染，同时减去 minL 带来的偏移，确保形状从自己的最左侧开始算起
        for (int i = 0; i < 4; i++) {
                int lx = shp::DATA[t][0][i][0];
                int ly = shp::DATA[t][0][i][1];

                // 关键：(lx - minL) 消除数据定义的起始位差异
                int sx = visualOffsetX + ((lx - minL) * 2);
                int sy = y + ly;

                DrawPoint(sx,     sy, GetColorByType(t), '[');
                DrawPoint(sx + 1, sy, GetColorByType(t), ']');
        }
}

// 1. 最基础的点位绘制
// 所有的字符和颜色最终都通过这里输出到屏幕
void GameView::DrawPoint(int x, int y, const char* color, char ch) {
        // 调用你之前封装的字符图形库接口
        // 注意：这里传入的是屏幕绝对坐标
        DrawPixelEx(x, y, ch, color);
}
void GameView::MapLogicToScreen(int lx, int ly, int& sx, int& sy) {
        // 逻辑坐标 lx (0-9) 映射到物理坐标 (BOARD_OFFSET_X 到 BOARD_OFFSET_X + 18)
        sx = BOARD_OFFSET_X + (lx * 2);
        sy = BOARD_OFFSET_Y + ly;
}

// 2. 水平线绘制
void GameView::DrawHLine(int x, int y, int length, char ch = '-') {
        for (int i = 0; i < length; i++) {
                DrawPoint(x + i, y, CG_COLOR_BLUE, ch); // 将外框改为蓝色
        }
}

// 2. 垂直线绘制
void GameView::DrawVLine(int x, int y, int length, char ch= '#') {
        for (int i = 0; i < length; i++) {
                DrawPoint(x, y + i, CG_COLOR_BLUE, ch);
        }
}

// 3. 形状绘制 (Tetromino)
// points 数组格式假设为: {x1, y1, x2, y2, x3, y3, x4, y4}
void GameView::DrawShape(int points[], int type) {
        const char* color = GetColorByType(type);

        for (int i = 0; i < 4; i++) {
                int px = points[i * 2];
                int py = points[i * 2 + 1];

                // 核心技巧：在控制台画方块，通常用两个字符 "[]" 组成一个正方形感官的点
                // 所以我们在 px 的位置画 '[', 在 px+1 的位置画 ']'
                DrawPoint(px,     py, color, '[');
                DrawPoint(px + 1, py, color, ']');
        }
}

void GameView::DrawTetromino(int type, int rotation, int startX, int startY) {
        int screenPoints[8];
        for (int i = 0; i < 4; i++) {
                // 核心职责：坐标换算
                screenPoints[i * 2]     = startX + (shp::DATA[type][rotation][i][0] * 2);
                screenPoints[i * 2 + 1] = startY + (shp::DATA[type][rotation][i][1]);
        }
        // 指派 DrawShape 去干活
        DrawShape(screenPoints, type);
}


const char* GameView::GetColorByType(int type) {
        static const char* colors[] = {
                CG_COLOR_CYAN,    // I
                CG_COLOR_YELLOW,  // O
                CG_COLOR_MAGENTA, // T
                CG_COLOR_GREEN,   // S
                CG_COLOR_RED,     // Z
                CG_COLOR_BLUE,    // J
                CG_COLOR_WHITE    // L
            };
        if (type >= 0 && type < 7) return colors[type];
        return CG_COLOR_WHITE;
}

// --- 布局组件实现 ---

// 在 DrawGameFrame 中统一绘制外壳
void GameView::DrawGameFrame() {
        int frameWidth = 40;  // 保持与 Render 一致
        int frameHeight = 22; // 20行 + 上下预留
        int startX = BOARD_OFFSET_X - 2;
        int startY = BOARD_OFFSET_Y - 1;

        char sym = '#';

        // 绘制大外框
        DrawHLine(startX, startY, frameWidth, sym);               // 顶边
        DrawHLine(startX, startY + frameHeight, frameWidth, sym); // 底边
        DrawVLine(startX, startY, frameHeight + 1, sym);          // 左边
        DrawVLine(startX + frameWidth - 1, startY, frameHeight + 1, sym); // 右边

        // 中隔墙
        int sepX = BOARD_OFFSET_X + 20;
        DrawVLine(sepX, startY, frameHeight + 1, sym);
}

void GameView::DrawInfoFrame() {
        // 2. 绘制信息区的垂直分隔线
        // 位于游戏区右侧，信息区左侧
        DrawVLine(23, 1, 21);
}

void GameView::DrawGameWall() {
        const int BOARD_W = 20; // 10列 * 2
        const int BOARD_H = 20; // 20行

        // 内部墙壁的 X 坐标
        int leftWallX = BOARD_OFFSET_X - 1;
        int rightWallX = BOARD_OFFSET_X + BOARD_W;

        // 内部地板的 Y 坐标
        int floorY = BOARD_OFFSET_Y + BOARD_H;

        // 1. 绘制垂直内墙：高度正好到 floorY 为止
        DrawVLine(leftWallX,  BOARD_OFFSET_Y, BOARD_H, '|');
        DrawVLine(rightWallX, BOARD_OFFSET_Y, BOARD_H, '|');

        // 2. 绘制底部地板：起点和终点要与垂直线重合
        // 起点设为 leftWallX，长度设为 (BOARD_W + 2) 刚好封死左右缺口
        DrawHLine(leftWallX, floorY, BOARD_W + 2, '-');
}
//信息区


void GameView::DrawScorePanel(const GameContext& ctx) {
    int scoreY = BOARD_OFFSET_Y + 1;
    // 缩短装饰线，PANEL_X + 1 起始
    DrawTextEx(PANEL_X + 1, scoreY, "── SCORE ──", CG_COLOR_CYAN);

    char buf[16];
    sprintf(buf, "%06d", ctx.score);
    // 分数稍微往右靠，实现居中感
    DrawTextEx(PANEL_X + 3, scoreY + 1, buf, CG_COLOR_WHITE);
}

void GameView::DrawStatusPanel(const GameContext& ctx) {
    int statusY = BOARD_OFFSET_Y + 4; // 与分数区空开
    DrawTextEx(PANEL_X + 1, statusY, "── STATUS ──", CG_COLOR_CYAN);

    const char* text = ctx.isGameOver ? "GAME OVER" : (ctx.isPaused ? "PAUSED" : "PLAYING");
    const char* color = ctx.isGameOver ? CG_COLOR_RED : (ctx.isPaused ? CG_COLOR_YELLOW : CG_COLOR_GREEN);

    // 文字起始点统一
    DrawTextEx(PANEL_X + 2, statusY + 1, text, color);
}

void GameView::DrawNextPiecePanel(const GameContext& ctx) {
    int nextY = BOARD_OFFSET_Y + 8; // 下移，给状态区留空间
    DrawTextEx(PANEL_X + 1, nextY, "── NEXT ──", CG_COLOR_CYAN);

    int type = ctx.nextPieceType % 7;
    // 这里的偏移量需要微调，确保方块在 NEXT 标签正下方居中
    int offsetX = PANEL_X + 4;
    int offsetY = nextY + 2;

    for (int i = 0; i < 4; i++) {
        int lx = shp::DATA[type][0][i][0];
        int ly = shp::DATA[type][0][i][1];
        DrawPoint(offsetX + (lx * 2), offsetY + ly, GetColorByType(type), '[');
        DrawPoint(offsetX + (lx * 2) + 1, offsetY + ly, GetColorByType(type), ']');
    }
}

void GameView::DrawNextPieceInPanel(int x, int y, int type) {
        int t = type % 7;
        // offsetX 稍微往右偏，让方块看起来在“NEXT”单词的中心线下
        int offsetX = x + 1;

        for (int i = 0; i < 4; i++) {
                int lx = shp::DATA[t][0][i][0];
                int ly = shp::DATA[t][0][i][1];
                DrawPoint(offsetX + (lx * 2), y + ly, GetColorByType(t), '[');
                DrawPoint(offsetX + (lx * 2) + 1, y + ly, GetColorByType(t), ']');
        }
}
//游戏区
void GameView::DrawStackedBlocks(const GameContext& ctx) {
        // 动画时长基准
        const float TOTAL_ANIM_TIME = 0.15f;

        for (int y = 0; y < 20; y++) {
                for (int x = 0; x < 10; x++) {
                        if (ctx.board[y][x] != 0) {
                                // --- 坐标换算 (替换你之前的坐标计算) ---
                                int sx = BOARD_OFFSET_X + x * 2;
                                int sy = BOARD_OFFSET_Y + y;

                                // --- 计算动态偏移 ---
                                int visualY = sy;
                                if (ctx.dropAnimTimer > 0) {
                                        // 剩下的时间比例 (1.0 递减到 0.0)
                                        float t = ctx.dropAnimTimer / TOTAL_ANIM_TIME;
                                        // 计算当前这一帧应该往上挪多少格（看起来就像是在掉落）
                                        // 这里的 rowOffsets[y] 记录的是这行在物理搬运前“下落了几格”
                                        visualY -= (int)(ctx.rowOffsets[y] * t + 0.5f);
                                }

                                // 只有在屏幕范围内才画
                                if (visualY >= BOARD_OFFSET_Y) {
                                        DrawPoint(sx, visualY, GetColorByType(ctx.board[y][x]-1), '[');
                                        DrawPoint(sx + 1, visualY, GetColorByType(ctx.board[y][x]-1), ']');
                                }
                        }
                }
        }
}
// 绘制当前正在下落的方块
void GameView::DrawActiveTetromino(const GameContext& ctx) {
        if (ctx.isGameOver) return;
        int sx, sy;
        MapLogicToScreen(ctx.curX, ctx.curY, sx, sy);
        DrawTetromino(ctx.curPieceType, ctx.curRotation, sx, sy);
}
// 私有辅助函数：将棋盘逻辑坐标转换为屏幕物理坐标
void GameView::DrawBoardPixel(int logicX, int logicY, char symbol) {
        DrawPixel(BOARD_OFFSET_X + logicX, BOARD_OFFSET_Y + logicY, symbol);
}

void GameView::DrawEffects(const GameContext& ctx) {
        // --- 第一部分：粒子物理模拟与渲染 ---
        // 假设每帧约 0.016s (60fps)，实际可根据你程序的主循环传入 deltaTime
        float dt = 0.016f;

        for (auto it = m_particles.begin(); it != m_particles.end(); ) {
                // 更新物理状态
                it->x += it->vx;
                it->y += it->vy;
                it->vy += 0.4f; // 重力：让粒子往下掉
                it->life -= dt;

                if (it->life <= 0) {
                        it = m_particles.erase(it);
                } else {
                        // 渲染粒子
                        int px = (int)it->x;
                        int py = (int)it->y;
                        // 简单边界检查，防止粒子飞出屏幕导致 DrawPoint 报错
                        if (py > 0 && py < 50 && px > 0 && px < 100) {
                                DrawPoint(px, py, it->color, it->ch);
                        }
                        ++it;
                }
        }

        // --- 第二部分：原有消行区域闪烁逻辑 ---
        if (ctx.lineClearTimer <= 0) return;

        // 只有在消行刚开始的瞬间（第一帧），生成粒子
        // 假设初始 timer 是 0.2f，在 0.19f 到 0.2f 之间触发一次生成
        static float lastTimer = 0;
        if (ctx.lineClearTimer > lastTimer && !ctx.linesToClear.empty()) {
                for (int rowY : ctx.linesToClear) {
                        CreateLineParticles(rowY, CG_COLOR_WHITE);
                }
        }
        lastTimer = ctx.lineClearTimer;

        // 以下是你原有的阶梯式闪烁代码
        const char* symbol = " ";
        const char* color = CG_COLOR_WHITE;

        if (ctx.lineClearTimer > 0.15f) {
                symbol = "#";
                color = CG_COLOR_WHITE;
        } else if (ctx.lineClearTimer > 0.08f) {
                symbol = "*";
                color = CG_COLOR_GRAY;
        } else {
                symbol = ".";
                color = CG_COLOR_GRAY;
        }

        for (int y : ctx.linesToClear) {
                for (int x = 0; x < 10; x++) {
                        int sx, sy;
                        MapLogicToScreen(x, y, sx, sy);
                        DrawPoint(sx, sy, color, symbol[0]);
                        DrawPoint(sx + 1, sy, color, symbol[0]);
                }
        }
}

void GameView::DrawModalBox(int x, int y, int w, int h, const char* color) {
        // 1. 清空背景（用空格填充，防止背景透出来乱糟糟的）
        for (int i = 0; i < h; i++) {
                for (int j = 0; j < w; j++) {
                        DrawPoint(x + j, y + i, color, ' ');
                }
        }
        // 2. 利用你之前的原子接口画框
        DrawHLine(x, y, w);             // 顶
        DrawHLine(x, y + h - 1, w);     // 底
        DrawVLine(x, y, h);             // 左
        DrawVLine(x + w - 1, y, h);     // 右
}

void GameView::DrawOverlay(const GameContext& ctx) {
        if (ctx.isGameOver) {
                // 调用通用框
                DrawModalBox(5, 8, 30, 6, CG_COLOR_RED);
                // 填入业务文字
                DrawTextEx(12, 10, " !!! GAME OVER !!! ", CG_COLOR_RED);
                DrawTextEx(8, 11, " Press R to Try Again ", CG_COLOR_WHITE);
        }
        else if (ctx.isPaused) {
                DrawModalBox(5, 8, 30, 6, CG_COLOR_YELLOW);
                DrawTextEx(14, 10, " PAUSED ", CG_COLOR_YELLOW);
                DrawTextEx(8, 11, " Press P to Resume ", CG_COLOR_WHITE);
        }
        // 增加：窗口太小强制警告
        if (IsLayoutInvalid()) {
                // 清理全屏，只显示警告（可选）
                ClearBackground();

                const char* msg = "!!! WINDOW TOO SMALL !!!";
                const char* hint = "Please resize the console to continue.";

                // 放在屏幕绝对坐标 (0, 0) 附近，确保玩家能看见
                DrawTextEx(2, 2, msg, CG_COLOR_RED);
                DrawTextEx(2, 3, hint, CG_COLOR_YELLOW);
        }
}

void GameView::SetColorByPieceType(int type) {
        // 1. 获取对应的颜色代码
        const char* color = GetColorByType(type);

        // 2. 直接向控制台发送颜色指令（如果你的库支持全局切换）
        // 或者存储在 GameView 的私有成员变量中，供下一次 Draw 调用
        this->currentDrawingColor = color;

        // 如果你的底层库是即时生效的：
        // printf("%s", color);
}

void GameView::DrawPiece(int type, int rot, int offsetX, int offsetY, bool isGhost) {
        // 1. 现场从 SHP 获取形状点集，不依赖 ctx 的缓存，也不需要逻辑层搬运
        shp::Point pts[4];
        shp::Get(type, rot, pts);

        for (int i = 0; i < 4; i++) {
                int drawX = offsetX + pts[i].x;
                int drawY = offsetY + pts[i].y;

                // 2. 渲染分支：你可以根据 isGhost 切换字符或颜色
                if (isGhost) {
                        DrawPixel(drawX, drawY, '.');
                } else {
                        DrawPixel(drawX, drawY, '#');
                }
        }
}
// 1. 专门画阴影的函数
void GameView::DrawGhost(const GameContext& ctx) {
        if (ctx.isGameOver || ctx.ghostY <= ctx.curY) return;

        for (int i = 0; i < 4; i++) {
                int lx = ctx.curX + shp::DATA[ctx.curPieceType][ctx.curRotation][i][0];
                int ly = ctx.ghostY + shp::DATA[ctx.curPieceType][ctx.curRotation][i][1];

                int sx, sy;
                MapLogicToScreen(lx, ly, sx, sy);

                // 使用半透明感的字符，不要用颜色，或者用深灰色
                DrawPoint(sx,     sy, CG_COLOR_GRAY, '.');
                DrawPoint(sx + 1, sy, CG_COLOR_GRAY, '.');
        }
}
void GameView::UpdateLayout(int screenWidth, int screenHeight) {
        const int TOTAL_WIDTH = 42;
        const int TOTAL_HEIGHT = 24; // 稍微增加预留高度

        // 计算居中偏移
        BOARD_OFFSET_X = (screenWidth - TOTAL_WIDTH) / 2;
        BOARD_OFFSET_Y = (screenHeight - TOTAL_HEIGHT) / 2;

        // --- 核心修复：防止偏移量变成负数 ---
        if (BOARD_OFFSET_X < 2) BOARD_OFFSET_X = 2;
        if (BOARD_OFFSET_Y < 2) BOARD_OFFSET_Y = 2; // 确保 Y 至少从第 2 行开始画

        // PANEL_X 也要基于 BOARD_OFFSET_X 重新对齐
        PANEL_X = BOARD_OFFSET_X + 21;
}
void GameView::CheckWindowSize() {

        int rows = GetScreenWidth();
        int cols = GetScreenHeight();

        // 俄罗斯方块至少需要约 40 宽 25 高
        if (cols < 40 || rows < 25) {
                printf("Please resize your console to at least 40x25 for the best experience.\n");
                // 可以选择暂停启动或强制设置窗口大小
        }
}
void GameView::DrawTextCentered(int centerX, int y, const char* text, const char* color) {
        int len = (int)strlen(text);
        // 这里的 centerX 是物理坐标（字符位）
        int sx = centerX - (len / 2);
        DrawTextEx(sx, y, text, color);
}
void GameView::CreateLineParticles(int logicY, const char* color) {
        for (int x = 0; x < 10; x++) {
                int sx, sy;
                MapLogicToScreen(x, logicY, sx, sy);

                // 每个方块生成 2 个粒子
                for (int i = 0; i < 2; i++) {
                        Particle p;
                        p.x = (float)sx + (i % 2);
                        p.y = (float)sy;

                        // 随机初速度：向左右迸射，并向上弹起
                        p.vx = ((float)(rand() % 100) - 50.0f) * 0.15f;
                        p.vy = -((float)(rand() % 60)) * 0.1f;

                        p.life = 0.6f; // 存活约 0.6 秒
                        p.color = color;
                        p.ch = (rand() % 2 == 0) ? '*' : '.';
                        m_particles.push_back(p);
                }
        }
}

bool GameView::IsLayoutInvalid() const {
        return (GetScreenWidth() < 42 || GetScreenHeight() < 25);
}

void GameView::DrawSizeWarning() {
        ClearBackground();
        DrawTextEx(2, 2, "!!! WINDOW TOO SMALL !!!", CG_COLOR_RED);
        // ... 其他提示文字
}