// GameView.h
#pragma once
#include "cg_gfx.h"
#include "GameContext.h"
#include <vector>

namespace {
    struct Particle;
}
class GameView {
public:
    // 1. 生命周期管理
    // 初始化控制台设置（如隐藏光标、设置窗口标题、初始化颜色对）
    GameView();
    // 恢复控制台默认设置
    ~GameView();
    // 2. 核心渲染接口
    // 这是 main 函数唯一需要调用的绘制函数
    void Render(const GameContext& ctx);
    bool IsLayoutInvalid() const;
    void DrawSizeWarning();

//private:
public:
    //1.Point/Cell
    void DrawPoint(int x, int y, const char*color, char);
    //作用：这是所有东西的基础。不管是画墙、画蛇、还是画俄罗斯方块，本质都是在某个坐标画一个点。
    //2线段 (Line/Border)：
    void DrawHLine(int x, int y, int length, char ch);
    void DrawVLine(int x, int y, int length, char ch);
    //利用循环调用 DrawPoint。主要用于画出游戏区域的边框。
    //3.方块/形状 (Tetromino)：
    void DrawShape(int points[], int type);
    //作用：接收 4 个坐标点，根据类型赋予颜色，一次性画出一个俄罗斯方块。
    // 绘制棋盘内已固化的方块 (ctx.board)
    void DrawStackedBlocks(const GameContext& ctx);
    // 绘制当前正在下落的活跃方块 (ctx.activePiece)
    void DrawActiveTetromino(const GameContext& ctx);

    //layout
    void DrawGameLayout();
    // 绘制游戏主边框（围墙）
    void DrawGameFrame();
    void DrawGameContent(const GameContext& ctx);
    void DrawInfoPanel(const GameContext& ctx);
    // 根据方块类型（1-7）设置控制台输出颜色
    void SetColorByPieceType(int type);
    // 内部绘制模块（分而治之）
    //游戏区
    //背景墙：由线段围成的矩形。
    void DrawGameWall();
    //特效层：消行时的闪烁动画（如果有）。
    void DrawEffects(const GameContext& ctx);
    //信息区
    void DrawInfoFrame();
    void DrawControlsPanel(int x, int y) ;
    void DrawNextPieceInPanel(int x, int y, int type);
    void DrawNextPieceCentered(int centerX, int y, int type);
    void DrawControlsCentered(int centerX, int y, const GameContext& ctx);
    void DrawTextCentered(int centerX, int y, const char* text, const char* color);
    //1.得分区
    void DrawScorePanel(const GameContext& ctx);
    //2.状态区
    void DrawStatusPanel(const GameContext& ctx);
    //3.提示区
    void DrawNextPiecePanel(const GameContext& ctx);
    const char* GetColorByType(int type);
    void DrawTetromino(int type, int rotation, int screenX, int screenY);

    void DrawModalBox(int x, int y, int w, int h, const char* color);
    // 绘制特殊的覆盖层：如“游戏结束”或“暂停”
    void DrawOverlay(const GameContext& ctx);
    void MapLogicToScreen(int lx, int ly, int& sx, int& sy);
    void DrawPiece(int type, int rot, int offsetX, int offsetY, bool isGhost);
    void DrawGhost(const GameContext& ctx);
    void DrawBoardPixel(int logicX, int logicY, char symbol);
    void UpdateLayout(int screenWidth, int screenHeight);
    void CheckWindowSize();
    void CreateLineParticles(int logicY, const char* color);
private:

    // 在 GameView 类私有成员中添加
    std::vector<Particle> m_particles;
    const char* currentDrawingColor=CG_COLOR_WHITE;
};

namespace {
    // 基础偏移（由 UpdateLayout 动态更新）
    int BOARD_OFFSET_X = 2;
    int BOARD_OFFSET_Y = 1;

    // 派生偏移（由 BOARD_OFFSET 算出）
    int PANEL_X = 26;

    // 固定逻辑尺寸
    const int BOARD_LOGIC_W = 10;
    const int BOARD_PHYSIC_W = 20; // 10格 * 2字符
    const int BOARD_HEIGHT = 20;
    struct Particle {
        float x, y;
        float vx, vy;
        float life;
        const char* color;
        char ch;
    };

}