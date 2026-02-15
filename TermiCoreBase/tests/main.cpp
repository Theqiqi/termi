#include <iostream>
#include <chrono>
#include <thread>
#include "TerminalGuard.h"

int main() {
    TerminalGuard guard;

    std::cout << "非阻塞模式启动。游戏循环运行中（按 'q' 退出）..." << std::endl;

    int FrameCount = 0;
    while (true) {
        // 模拟游戏逻辑：每帧累加计数并打印
        std::cout << "\r当前帧数: " << FrameCount++ << " [按键监听中...] " << std::flush;

        // 尝试获取输入
        int key = guard.readKey();
        if (key == 'q' || key == 'Q') {
            std::cout << "\n检测到退出信号。" << std::endl;
            break;
        }

        // 限制循环速度，大约 60 FPS
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}