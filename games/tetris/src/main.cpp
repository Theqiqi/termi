

#include "ray_console.h"  // 你的字符引擎封装
#include "GameContext.h"    // 纯数据结构
#include "GameEngine.h"

int main() {

    SetTargetFPS(60);
    InitConsole(40, 30);

    GameEngine engine;
    engine.Run();
    return 0;

}