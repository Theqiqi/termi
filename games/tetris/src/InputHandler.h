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

        // 处理退出、暂停、AI切换等全局键
    void ProcessSystemKeys(GameContext& ctx, int key) {

        if (key == 27) ctx.shouldExit = true;
        if (key == 'p' || key == 'P') ctx.isPaused = !ctx.isPaused;
        if (key == 'm' || key == 'M') ctx.isAIMode = !ctx.isAIMode;
            if (ctx.isGameOver && (key == 'r' || key == 'R')) /* 触发重启 */;
        }

        // 仅在手动玩游戏时调用
    void ProcessGameKeys(GameContext& ctx, GameLogic& logic, int key) {
        // 此时 key 是由 Engine 传进来的，不要在函数内部 GetKeyPressed()
        switch (key) {
            case KEY_LEFT:  case 'a': case 'A': logic.Move(ctx, -1, 0); break;
            case KEY_RIGHT: case 'd': case 'D': logic.Move(ctx, 1, 0);  break;
            case KEY_DOWN:  case 's': case 'S': logic.Move(ctx, 0, 1);  break;
            case KEY_UP:    case 'w': case 'W': logic.TryRotate(ctx);   break;
            case KEY_SPACE: logic.HardDrop(ctx); break;
        }

#ifdef GAME_DEBUG
        cheat(ctx, logic, key);
#endif
    }

};