#include "ray_console.h"
#include "GameLogic.h"
#include "key.h"


class InputHandler {
private:
    float m_inputTimer = 0.0f; // 移动到私有成员，保持持久化
    const float INPUT_DELAY = 0.10f;
public:
#ifdef GAME_DEBUG
    void cheat(GameContext& ctx, GameLogic& logic,int key){
        switch (key) {
            case KEY_F1:
            {
                // 一键填满第 19 行，只留一个洞，方便测试消行
                for(int x=0; x<9; x++) ctx.board[19][x] = 1;
                ctx.board[19][9] = 0;
                break;
            }
            case KEY_F2:
            {
                // 一键自杀，测试死亡 UI
                ctx.isGameOver = true;
                break;
            }
            case KEY_F3:
            {
                if (logic.Move(ctx, 0, 1)) {
                    ctx.dropTimer = 0.0f; // 手动下落后重置重力计时
                }
                break;
            }
            case KEY_F4:
            {
                // 增加等级/速度，测试高难度渲染
                ctx.score += 5000;
                logic.ApplyScoreAndFeedback(ctx, 0);
                break;
            }

        }

    }

#endif
    // 将输入转换为对逻辑层的具体指令
    void ProcessInput(GameContext& ctx, GameLogic& logic) {
        // 1. 获取当前这一帧的按键
        int key = GetKeyPressed();
        if (key <= 0) return; // 没按键直接返回

        // 2. 基础控制
        if (key == 27) { ctx.shouldExit = true; return; }
        if (key == 'p' || key == 'P') { ctx.isPaused = !ctx.isPaused; return; }
        if (ctx.isGameOver) {
            if (key == 'r' || key == 'R') logic.Reset(ctx);
            return;
        }
        if (ctx.isPaused || ctx.lineClearTimer > 0) return;

        // 3. 响应移动（使用 TryMove 彻底解决穿墙）
        // 注意：这里直接用 key 匹配，不重复写 if
        switch (key) {
            case KEY_LEFT:  case 'a': case 'A':
                logic.Move(ctx, -1, 0);
                break;
            case KEY_RIGHT: case 'd': case 'D':
                logic.Move(ctx, 1, 0);
                break;
            case KEY_DOWN:  case 's': case 'S':
                logic.Move(ctx, 0, 1);
                break;
            case KEY_UP:    case 'w': case 'W':
                logic.TryRotate(ctx);
                break;
            case KEY_SPACE: // 这里已经包含了 ' ' (32)，不要再重复写 case ' ':
                logic.HardDrop(ctx);
                break;
            case 'f': case 'F':
                ctx.score += 1000;
                break;
        }

#ifdef GAME_DEBUG
        cheat(ctx, logic, key);
#endif
    }

};