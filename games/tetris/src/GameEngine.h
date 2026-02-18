//
// Created by kali on 2/18/26.
//
#ifndef MYCONSOLEGAMES_GAMEENGINE_H
#define MYCONSOLEGAMES_GAMEENGINE_H

#include "GameContext.h"    // 纯数据结构
#include "GameLogic.h"      // 纯逻辑处理
#include "GameView.h"       // 字符渲染处理
#include "InputHandler.h"
#include "TimeTicker.h"
#include "AIController.h"
#include "AudioManager.h"


class GameEngine {
public:
    GameEngine(int targetFPS = 60);   // 默认参数写在这里

    void GenerateFeedback();
    void UpdatePhysics(float dt);
    void GatherInput();
    void ProcessGameFrame(float dt);
    void PostUpdate();
    void Run();
    void UpdateSystemState();
private:

    // 所有组件作为成员变量
    GameContext  m_ctx;
    GameLogic    m_logic;
    GameView     m_view;
    AudioManager m_audio;
    InputHandler m_input;
    TimeTicker   m_ticker;
    AIController m_ai;
};

#endif //MYCONSOLEGAMES_GAMEENGINE_H
