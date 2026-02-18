#include "LogSystem.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <vector>

namespace LogSystem {
    // 静态变量：存储在内存中的日志
    static std::vector<LogEntry> g_logs;

    // 启动时调用一次，清空上一次运行的 log 文件，避免文件无限变大
    void InitDebugFile() {
        std::ofstream outFile("game_debug.log", std::ios::trunc); // trunc = 清空重写
        if (outFile.is_open()) {
            outFile << "=== Game Started ===" << std::endl;
            outFile.close();
        }
    }

    void Log(const std::string& msg) {
        // --- 1. 获取当前时间 ---
        float now = (float)clock() / CLOCKS_PER_SEC;

        // --- 2. 写入内存 (用于屏幕渲染) ---
        if (g_logs.size() >= MAX_LOGS) {
            g_logs.erase(g_logs.begin()); // 挤掉最旧的一条
        }
        g_logs.push_back({ msg, now });

        // --- 3. 写入文件 (用于死机排查) ---
        // 使用 append 模式，每一条都追加到文件末尾
        std::ofstream outFile("game_debug.log", std::ios::app);
        if (outFile.is_open()) {
            outFile << "[" << now << "] " << msg << std::endl;
            // 这里的 close 会强制刷新缓冲区，确保死机前数据能存进硬盘
            outFile.close();
        }

        // --- 4. (可选) 输出到 IDE 控制台 ---
        std::cout << "[LOG] " << msg << std::endl;
    }

    const std::vector<LogEntry>& GetEntries() {
        return g_logs;
    }

    void Clear() {
        g_logs.clear();
    }
}