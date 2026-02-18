#pragma once
#include <vector>
#include <string>

namespace LogSystem {
    // 日志条目结构
    struct LogEntry {
        std::string message;
        float timestamp;
    };

    // 配置：内存中保留多少条日志（用于屏幕显示）
    const int MAX_LOGS = 10;

    // 核心接口
    void Log(const std::string& msg);              // 记日志
    const std::vector<LogEntry>& GetEntries();     // 获取内存日志（供渲染用）
    void Clear();                                  // 清空内存日志
    void InitDebugFile();                          // 【新】启动时清空旧的 log 文件
}