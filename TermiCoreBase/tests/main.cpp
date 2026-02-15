#include <iostream>
#include <chrono>
#include <thread>
#include "TerminalGuard.h"
#include "TerminalTopology.h"

int main() {
    TerminalGuard guard; // 之前的 T1.1 & T1.2
    TerminalTopology topology;

    std::cout << "拓扑感知启动。请尝试拉伸终端窗口（按 'q' 退出）..." << std::endl;

    while (true) {
        if (topology.check_and_update()) {
            std::cout << "\n[检测到窗口变化] 新分辨率: "
                      << topology.get_width() << "x" << topology.get_height() << std::endl;
        }

        int key = guard.readKey();
        if (key == 'q' || key == 'Q') break;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}