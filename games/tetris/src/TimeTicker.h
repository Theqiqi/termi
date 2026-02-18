#include <chrono>
#include <thread>

class TimeTicker {
public:
    TimeTicker(int targetFPS = 60) 
        : targetFPS(targetFPS), 
          frameDuration(1.0f / targetFPS),
          lastTime(std::chrono::high_resolution_clock::now()) {}

    // 核心方法：计算两帧间隔，并确保不超过目标帧率
    float Tick() {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        
        // 如果跑得太快，就在这里等一下 (FPS 控制)
        if (elapsed.count() < frameDuration) {
            float sleepTime = frameDuration - elapsed.count();
            std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
            
            // 重新计算等待后的时间
            currentTime = std::chrono::high_resolution_clock::now();
            elapsed = currentTime - lastTime;
        }

        lastTime = currentTime;
        return elapsed.count(); // 返回最终的 dt
    }

private:
    int targetFPS;
    float frameDuration;
    std::chrono::high_resolution_clock::time_point lastTime;
};