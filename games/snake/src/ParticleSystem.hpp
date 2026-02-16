// games/snake/src/ParticleSystem.hpp
#pragma once
#include <vector>
#include "ray_console.h" // 仅依赖游戏库 API
#include <cstdio>
struct Particle {
    float x, y, vx, vy;
    int life;
    const char* color;
};

class ParticleSystem {
public:
    void Emit(float x, float y, int count, const char* color) {
        for (int i = 0; i < count; ++i) {
            particles.push_back({
                x, y,
                (float)(rand() % 20 - 10) * 0.8f, // 显著加大水平速度
                (float)(rand() % 20 - 10) * 0.4f, // 显著加大垂直速度
                15,
                color
            });
        }
    }

    void Update() {
        //fprintf(stderr,"Particles count: %lu\n", particles.size()); // 调试用
        for (auto it = particles.begin(); it != particles.end(); ) {
            it->x += it->vx;
            it->y += it->vy;
            // 关键：每一帧损失 10% 的速度
            it->vx *= 0.92f;
            it->vy *= 0.92f;

            it->life--;
            // it->vy += 0.02f; // 如果你想加点重力感
            if (--it->life <= 0) it = particles.erase(it);
            else ++it;
        }
    }

    void Render() {
        for (const auto& p : particles) {
            DrawPixelEx((int)p.x, (int)p.y, '#', p.color);
        }
    }
    size_t GetCount() const {
        return particles.size();
    }
private:
    std::vector<Particle> particles;
};