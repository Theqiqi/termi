#include <sys/ioctl.h>
#include <signal.h>
#include <atomic>

// 全局原子变量，确保信号处理的线程安全
std::atomic<bool> screen_resized{true};

// 信号处理回调函数
void handle_resize(int sig) {
    screen_resized.store(true);
}

class TerminalTopology {
public:
    TerminalTopology() : width(0), height(0) {
        // 注册信号处理器
        signal(SIGWINCH, handle_resize);
        update_dimensions();
    }

    // 检查并更新尺寸
    bool check_and_update() {
        if (screen_resized.load()) {
            update_dimensions();
            screen_resized.store(false);
            return true; // 告知外部：布局已变，需要重新分配 Buffer
        }
        return false;
    }

    int get_width() const { return width; }
    int get_height() const { return height; }

private:
    int width, height;

    void update_dimensions() {
        struct winsize ws;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1) {
            width = ws.ws_col;
            height = ws.ws_row;
        }
    }
};