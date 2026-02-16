#include <queue>
#include <set>
#include <vector>
#include <cmath>

// 辅助结构体：记录路径与坐标
struct Node {
    int x, y;
    std::vector<Direction> path;
};

Direction CalculateBFSMove(SnakeGame& game) {
    // 1. 获取实时逻辑参数
    int startX = game.GetHeadX();
    int startY = game.GetHeadY();
    int foodX = game.GetFoodX();
    int foodY = game.GetFoodY();
    int limitW = game.GetWidth();
    int limitH = game.GetHeight();

    // 2. 初始化队列与访问集
    std::queue<Node> q;
    q.push({startX, startY, {}});

    std::set<std::pair<int, int>> visited;
    visited.insert({startX, startY});

    // 3. 定义动作向量：必须适配你的步长（横向2，纵向1）
    struct Move { Direction d; int dx, dy; };
    std::vector<Move> moves = {
        {Direction::UP,    0, -1},
        {Direction::DOWN,  0,  1},
        {Direction::LEFT, -2,  0},
        {Direction::RIGHT, 2,  0}
    };

    while (!q.empty()) {
        Node curr = q.front();
        q.pop();

        // 4. 判定目标：由于步长容差，使用与 Update 相同的 abs <= 1 判定
        if (std::abs(curr.x - foodX) <= 1 && curr.y == foodY) {
            // 如果路径为空，说明已经在食物上，返回当前第一个动作
            return curr.path.empty() ? Direction::RIGHT : curr.path[0];
        }

        // 5. 遍历四个方向
        for (const auto& m : moves) {
            int nextX = curr.x + m.dx;
            int nextY = curr.y + m.dy;

            // --- 核心修正：碰撞预判 ---

            // A. 墙体检查：必须避开逻辑边界坐标 (0 和 w-1)
            if (nextX <= 0 || nextX >= limitW - 1 || nextY <= 0 || nextY >= limitH - 1) {
                continue;
            }

            // B. 蛇身检查：调用你刚才写的 IsSnakeAt 接口
            if (game.IsSnakeAt(nextX, nextY)) {
                continue;
            }

            // C. 访问去重
            if (visited.find({nextX, nextY}) == visited.end()) {
                visited.insert({nextX, nextY});

                std::vector<Direction> nextPath = curr.path;
                nextPath.push_back(m.d);
                q.push({nextX, nextY, nextPath});
            }
        }
    }

    // 6. 逃生逻辑：如果 BFS 找不到通往食物的路
    // 尝试找一个不会立刻撞墙的方向（按右、下、左、上的顺序尝试一个空位）
    for (const auto& m : moves) {
        int nx = startX + m.dx;
        int ny = startY + m.dy;
        if (nx > 0 && nx < limitW - 1 && ny > 0 && ny < limitH - 1 && !game.IsSnakeAt(nx, ny)) {
            return m.d;
        }
    }

    return Direction::RIGHT; // 彻底无路可走，听天由命
}



void TestAutoPlay() {
    std::cout << "\n>>> AI Pilot Stress Test Start <<<\n";
    SnakeGame game(40, 20); // 宽 40 高 20 是一个很稳的测试比例

    int movesLimit = 1000;
    while (movesLimit-- > 0 && !game.IsGameOver()) {
        Direction nextMove = CalculateBFSMove(game);
        game.HandleInput(nextMove); // 使用正式接口

        bool ateFood = game.Update();

        if (ateFood) {
            std::cout << "SUCCESS: AI captured food! Score: " << game.GetScore() << std::endl;
        }
    }

    if (game.IsGameOver()) {
        std::cout << "CRASH: AI hit a wall or itself at step " << 1000 - movesLimit << std::endl;
        // 打印死亡坐标方便调试
        std::cout << "Died at: (" << game.GetHeadX() << "," << game.GetHeadY() << ")" << std::endl;
    }
}