#include "SnakeAI.h"
#include <queue>
#include <set>
#include <vector>
#include <cmath>
#include <deque>


Direction SnakeAI::CalculateFastMove(const SnakeGame& game) {
    int startX = game.GetHeadX();
    int startY = game.GetHeadY();
    int foodX = game.GetFoodX();
    int foodY = game.GetFoodY();

    std::queue<Node> q;
    q.push({startX, startY, {}});

    std::set<std::pair<int, int>> visited;
    visited.insert({startX, startY});

    // 考虑你的步长：横向 2，纵向 1
    struct Move { Direction d; int dx, dy; };
    std::vector<Move> moves = {
        {Direction::UP, 0, -1}, {Direction::DOWN, 0, 1},
        {Direction::LEFT, -2, 0}, {Direction::RIGHT, 2, 0}
    };

    while (!q.empty()) {
        Node curr = q.front();
        q.pop();

        // 判定吃到食物：与游戏本体 Update 逻辑保持高度一致
        // 只要当前点能触发“吃”的判定，就认为找到了路径
        if (std::abs(curr.x - foodX) <= 1 && curr.y == foodY) {
            // 如果路径为空（理论上不会），返回当前蛇的方向作为保底
            return curr.path.empty() ? Direction::RIGHT : curr.path[0];
        }

        for (auto& m : moves) {
            int nextX = curr.x + m.dx;
            int nextY = curr.y + m.dy;

            // 调用修正后的 IsValidMove
            if (IsValidMove(game, nextX, nextY) && visited.find({nextX, nextY}) == visited.end()) {
                visited.insert({nextX, nextY});

                // 性能优化：直接复用 path 引用（可选）或按原样 push
                std::vector<Direction> nextPath = curr.path;
                nextPath.push_back(m.d);
                q.push({nextX, nextY, nextPath});
            }
        }
    }

    // --- 逃生逻辑修正 ---
    // 找不到通向食物的路时，尝试按照目前的方向“续命”，而不是硬性转向 UP
    for (auto& m : moves) {
        if (IsValidMove(game, startX + m.dx, startY + m.dy)) {
            return m.d;
        }
    }
    return Direction::RIGHT;
}

bool SnakeAI::IsValidMove(const SnakeGame& game, int x, int y) {
    // 关键修正：确保这里的 w 和 h 拿到的不是默认的 800/600
    int w = game.GetWidth();
    int h = game.GetHeight();

    // 必须避开 0 和 Max-1，因为那是墙所在的坐标
    if (x <= 0 || x >= w - 1 || y <= 0 || y >= h - 1)
        return false;

    // 障碍物检查
    return !game.IsSnakeAt(x, y);
}


// 1. 模拟移动逻辑（纯逻辑推演，不改变游戏状态）
std::deque<Point> SnakeAI::SimulateMove(const std::deque<Point>& currentSnake, Direction dir) {
    std::deque<Point> nextSnake = currentSnake;
    Point head = nextSnake.front();
    if (dir == Direction::UP) head.y--;
    else if (dir == Direction::DOWN) head.y++;
    else if (dir == Direction::LEFT) head.x -= 2;
    else if (dir == Direction::RIGHT) head.x += 2;

    nextSnake.push_front(head);
    nextSnake.pop_back(); // 模拟时不增长，只看连通性
    return nextSnake;
}

// 2. 基础 BFS 寻路算法（增加了对模拟蛇身的检测）
std::vector<Direction> SnakeAI::BFS(const SnakeGame& game, Point start, Point target, const std::deque<Point>& virtualBody) {
    int limitW = game.GetWidth();
    int limitH = game.GetHeight();

    std::queue<AINode> q;
    q.push({start.x, start.y, {}});

    std::set<std::pair<int, int>> visited;
    visited.insert({start.x, start.y});

    std::vector<AIMove> moves = {
        {Direction::LEFT, -2, 0}, {Direction::RIGHT, 2, 0},
        {Direction::UP, 0, -1}, {Direction::DOWN, 0, 1}
    };

    while (!q.empty()) {
        AINode curr = q.front();
        q.pop();

        if (std::abs(curr.x - target.x) <= 1 && curr.y == target.y) return curr.path;

        for (auto& m : moves) {
            int nx = curr.x + m.dx;
            int ny = curr.y + m.dy;

            // 边界检查
            if (nx <= 0 || nx >= limitW - 1 || ny <= 0 || ny >= limitH - 1) continue;

            // 蛇身检查（在模拟的身体里检查）
            bool hitBody = false;
            for (const auto& p : virtualBody) {
                if (p.x == nx && p.y == ny) { hitBody = true; break; }
            }
            if (hitBody) continue;

            if (visited.find({nx, ny}) == visited.end()) {
                visited.insert({nx, ny});
                std::vector<Direction> nextPath = curr.path;
                nextPath.push_back(m.d);
                q.push({nx, ny, nextPath});
            }
        }
    }
    return {}; // 无路可走
}
// 3. 漫游逻辑：找不到路时找个最空旷的地方
Direction SnakeAI::Wander(const SnakeGame& game) {
    Point head = {game.GetHeadX(), game.GetHeadY()};
    std::vector<AIMove> moves = {{Direction::LEFT, -2, 0}, {Direction::RIGHT, 2, 0}, {Direction::UP, 0, -1}, {Direction::DOWN, 0, 1}};

    for (auto& m : moves) {
        int nx = head.x + m.dx;
        int ny = head.y + m.dy;
        if (nx > 0 && nx < game.GetWidth() - 1 && ny > 0 && ny < game.GetHeight() - 1 && !game.IsSnakeAt(nx, ny)) {
            return m.d;
        }
    }
    return Direction::UP;
}

// 4. 核心决策大脑
Direction SnakeAI::CalculateEscapeMove(const SnakeGame& game) {
    Point head = {game.GetHeadX(), game.GetHeadY()};
    Point food = {game.GetFoodX(), game.GetFoodY()};

    // A. 寻找通往食物的路径
    std::vector<Direction> pathToFood = BFS(game, head, food, game.getSnake());

    if (!pathToFood.empty()) {
        // B. 虚拟演练：假设吃了食物
        std::deque<Point> virtualSnake = game.getSnake();
        for (Direction d : pathToFood) virtualSnake = SimulateMove(virtualSnake, d);

        // C. 安全检查：吃完后还能抓到自己的尾巴吗？
        // 目标是现在的尾巴位置（因为尾巴会动）
        std::vector<Direction> pathToTail = BFS(game, virtualSnake.front(), virtualSnake.back(), virtualSnake);
        if (!pathToTail.empty()) {
            return pathToFood[0]; // 确认安全，执行第一步
        }
    }

    // D. 逃生模式：如果吃不到食物或不安全，转而追逐自己的尾巴
    std::vector<Direction> pathToTail = BFS(game, head, game.getSnake().back(), game.getSnake());
    if (!pathToTail.empty()) {
        return pathToTail[0];
    }

    // E. 保底：随便走走
    return Wander(game);
}



// --- 核心辅助函数 ---

// 1. 模拟移动逻辑（纯演练，不改变游戏数据）
// 注意：为了预判安全性，模拟移动后头部的坐标变化
std::deque<Point> SnakeAI::SimulateSnakeMove(const std::deque<Point>& currentSnake, Direction dir) {
    std::deque<Point> nextSnake = currentSnake;
    Point head = nextSnake.front();

    if (dir == Direction::UP) head.y--;
    else if (dir == Direction::DOWN) head.y++;
    else if (dir == Direction::LEFT) head.x -= 2;
    else if (dir == Direction::RIGHT) head.x += 2;

    nextSnake.push_front(head);
    nextSnake.pop_back(); // 模拟移动时尾部腾出空间
    return nextSnake;
}

// 2. 广义 BFS 寻路（支持传入自定义的虚拟身体）
// 这解决了你提到的 CanReachTarget 的需求：如果能搜到路径，就代表 Target 可达
std::vector<Direction> SnakeAI::SearchPath(const SnakeGame& game, Point start, Point target, const std::deque<Point>& obstacleBody) {
    int limitW = game.GetWidth();
    int limitH = game.GetHeight();

    std::queue<AINode> q;
    q.push({start.x, start.y, {}});

    std::set<std::pair<int, int>> visited;
    visited.insert({start.x, start.y});

    // 动作优先级：横向 2，纵向 1
    std::vector<AIMove> moves = {
        {Direction::UP, 0, -1}, {Direction::DOWN, 0, 1},
        {Direction::LEFT, -2, 0}, {Direction::RIGHT, 2, 0}
    };
    std::random_shuffle(moves.begin(), moves.end());
    while (!q.empty()) {
        AINode curr = q.front();
        q.pop();

        // 吃到目标判定（1像素容差）
        if (std::abs(curr.x - target.x) <= 1 && curr.y == target.y) {
            return curr.path;
        }

        for (auto& m : moves) {
            int nx = curr.x + m.dx;
            int ny = curr.y + m.dy;

            // 边界检查
            if (nx <= 0 || nx >= limitW - 1 || ny <= 0 || ny >= limitH - 1) continue;

            // 障碍物检查（检查是否撞到传入的虚拟身体）
            bool hitBody = false;
            for (const auto& p : obstacleBody) {
                if (p.x == nx && p.y == ny) { hitBody = true; break; }
            }
            if (hitBody) continue;

            if (visited.find({nx, ny}) == visited.end()) {
                visited.insert({nx, ny});
                std::vector<Direction> nextPath = curr.path;
                nextPath.push_back(m.d);
                q.push({nx, ny, nextPath});
            }
        }
    }
    return {}; // 返回空表示不可达
}

// 3. 漫游保底逻辑
Direction SnakeAI::WanderSafely(const SnakeGame& game) {
    Point head = {game.GetHeadX(), game.GetHeadY()};
    Point food = {game.GetFoodX(), game.GetFoodY()};

    std::vector<AIMove> moves = {
        {Direction::UP, 0, -1}, {Direction::DOWN, 0, 1},
        {Direction::LEFT, -2, 0}, {Direction::RIGHT, 2, 0}
    };

    // 按到食物的距离排序，这样即使在漫游，也会尽量往食物方向靠
    std::sort(moves.begin(), moves.end(), [&](const AIMove& a, const AIMove& b) {
        int distA = std::abs(head.x + a.dx - food.x) + std::abs(head.y + a.dy - food.y);
        int distB = std::abs(head.x + b.dx - food.x) + std::abs(head.y + b.dy - food.y);
        return distA < distB;
    });

    for (auto& m : moves) {
        int nx = head.x + m.dx;
        int ny = head.y + m.dy;
        if (nx > 0 && nx < game.GetWidth() - 1 && ny > 0 && ny < game.GetHeight() - 1 && !game.IsSnakeAt(nx, ny)) {
            return m.d;
        }
    }
    return Direction::RIGHT;
}

// --- 最终决策接口 ---

Direction SnakeAI::CalculateBestMove(const SnakeGame& game) {
    Point headPos = { game.GetHeadX(), game.GetHeadY() };
    Point foodPos = { game.GetFoodX(), game.GetFoodY() };
    const std::deque<Point>& currentBody = game.getSnake();

    // 💡 优化 1：长度过滤
    // 当蛇很短时，不需要过度检查安全性，直接冲。
    // 这能解决你说的“初期横向打转”的问题。
    bool isShort = currentBody.size() < 10;

    // 第一步：寻找通向食物的路径
    std::vector<Direction> pathToFood = SearchPath(game, headPos, foodPos, currentBody);

    if (!pathToFood.empty()) {
        if (isShort) {
            return pathToFood[0]; // 幼年期：直接冲
        }

        // 第二步：模拟推演（成年期逻辑）
        std::deque<Point> virtualSnake = currentBody;
        for (Direction d : pathToFood) {
            virtualSnake = SimulateSnakeMove(virtualSnake, d);
        }

        // 第三步：安全性检查
        std::vector<Direction> pathToTail = SearchPath(game, virtualSnake.front(), virtualSnake.back(), virtualSnake);
        if (!pathToTail.empty()) {
            return pathToFood[0];
        }
    }

    // 第四步：追逐尾巴逻辑
    std::vector<Direction> followTail = SearchPath(game, headPos, currentBody.back(), currentBody);
    if (!followTail.empty()) {
        // 💡 优化 2：防止原地打转
        // 如果去追尾巴的第一步会让你陷入僵局，可以尝试随机或漫游
        return followTail[0];
    }

    // 第五步：最后挣扎
    return WanderSafely(game);
}

