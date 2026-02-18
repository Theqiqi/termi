class AudioManager {
public:
    // 只传入 ctx 引用，纯读取，不修改
    void Update(const GameContext& ctx) {

        // 1. 捕捉消行瞬间
        if (ctx.lineClearedEvent) {
            // 这里可以根据 ctx.linesToClear.size() 播放不同音效
            PlayConsoleSound();
        }

        // 2. 捕捉游戏结束瞬间
        if (ctx.isGameOver && !m_playedGameOver) {
            // PlayGameOverSound();
            m_playedGameOver = true;
        }

        // 如果游戏重启了，重置标志位
        if (!ctx.isGameOver) m_playedGameOver = false;
    }

private:
    bool m_playedGameOver = false;
};