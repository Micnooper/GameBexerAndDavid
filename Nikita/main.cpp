#include <deque>
#include <iostream>
#include <memory>

#include <raylib.h>
#include <raymath.h>

//  constants
namespace {
    constexpr Color  kGreen      = {173, 204, 96, 255};
    constexpr Color  kDarkGreen  = { 43,  51, 24, 255};
    constexpr Color  kDarkRed    = {139,   0,  0, 255};
    constexpr int    kCellSize   = 30;
    constexpr int    kCellCount  = 25;
    constexpr int    kOffset     = 75;
    constexpr double kTickRate   = 0.1;
}

//  helpers 
[[nodiscard]]
bool elementInDeque(const Vector2& element,
                    const std::deque<Vector2>& dq)
{
    for (const auto& item : dq) {
        if (Vector2Equals(item, element)) return true;
    }
    return false;
}

[[nodiscard]]
bool eventTriggered(double interval)
{
    static double lastUpdate {};
    const double  now         {GetTime()};
    if (now - lastUpdate >= interval) {
        lastUpdate = now;
        return true;
    }
    return false;
}

//  Snake 
class Snake {
public:
    Snake()  = default;
    ~Snake() = default;

    Snake(const Snake&)            = delete;
    Snake& operator=(const Snake&) = delete;
    Snake(Snake&&)                 = default;
    Snake& operator=(Snake&&)      = default;

    void draw() const
    {
        for (const auto& seg : body) {
            const Rectangle rect{
                static_cast<float>(kOffset) + seg.x * kCellSize,
                static_cast<float>(kOffset) + seg.y * kCellSize,
                static_cast<float>(kCellSize),
                static_cast<float>(kCellSize)
            };
            DrawRectangleRounded(rect, 0.5f, 6, kDarkGreen);
        }
    }

    void update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if (!addSegment) body.pop_back();
        addSegment = false;
    }

    void reset()
    {
        body       = {{6, 9}, {5, 9}, {4, 9}};
        direction  = {1, 0};
        addSegment = false;
    }

    std::deque<Vector2> body{{6, 9}, {5, 9}, {4, 9}};
    Vector2             direction{1, 0};
    bool                addSegment = false;
};

//  Food 
class Food {
public:
    explicit Food(const std::deque<Vector2>& snakeBody)
        : position_{generateRandomPos(snakeBody)}
    {}

    ~Food() = default;

    Food(const Food&)            = delete;
    Food& operator=(const Food&) = delete;
    Food(Food&&)                 = default;
    Food& operator=(Food&&)      = default;

    void draw() const
    {
        const float cx {kOffset + position_.x * kCellSize + kCellSize / 2.0f};
        const float cy {kOffset + position_.y * kCellSize + kCellSize / 2.0f};
        DrawCircle(static_cast<int>(cx),
                   static_cast<int>(cy),
                   kCellSize / 2.0f,
                   kDarkRed);
    }

    void respawn(const std::deque<Vector2>& snakeBody)
    {
        position_ = generateRandomPos(snakeBody);
    }

    [[nodiscard]] Vector2 position() const { return position_; }

private:
    [[nodiscard]]
    static Vector2 generateRandomCell()
    {
        return {
            static_cast<float>(GetRandomValue(0, kCellCount - 1)),
            static_cast<float>(GetRandomValue(0, kCellCount - 1))
        };
    }

    [[nodiscard]]
    static Vector2 generateRandomPos(const std::deque<Vector2>& snakeBody)
    {
        Vector2 pos = generateRandomCell();
        while (elementInDeque(pos, snakeBody))
            pos = generateRandomCell();
        return pos;
    }

    Vector2 position_ {};
};

//  Game 
class Game {
public:
    Game()  = default;
    ~Game() = default;

    Game(const Game&)            = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&&)                 = default;
    Game& operator=(Game&&)      = default;

    void draw() const
    {
        food_.draw();
        snake_.draw();
    }

    void update()
    {
        if (!running_) return;
        snake_.update();
        checkCollisionWithFood();
        checkCollisionWithEdges();
        checkCollisionWithTail();
    }

    void processInput()
    {
        if (IsKeyPressed(KEY_UP)    && snake_.direction.y !=  1 && allowMove_)
            changeDirection({0, -1});
        if (IsKeyPressed(KEY_DOWN)  && snake_.direction.y != -1 && allowMove_)
            changeDirection({0,  1});
        if (IsKeyPressed(KEY_LEFT)  && snake_.direction.x !=  1 && allowMove_)
            changeDirection({-1, 0});
        if (IsKeyPressed(KEY_RIGHT) && snake_.direction.x != -1 && allowMove_)
            changeDirection({1,  0});
    }

    void setAllowMove(bool v) { allowMove_ = v; }
    [[nodiscard]] int  score()   const { return score_;   }

private:
    void changeDirection(Vector2 dir)
    {
        snake_.direction = dir;
        running_   = true;
        allowMove_ = false;
    }

    void checkCollisionWithFood()
    {
        if (Vector2Equals(snake_.body[0], food_.position())) {
            food_.respawn(snake_.body);
            snake_.addSegment = true;
            ++score_;
        }
    }

    void checkCollisionWithEdges()
    {
        const auto& h {snake_.body[0]};
        if (h.x < 0 || h.x >= kCellCount ||
            h.y < 0 || h.y >= kCellCount)
            gameOver();
    }

    void checkCollisionWithTail()
    {
        auto headless {snake_.body};
        headless.pop_front();
        if (elementInDeque(snake_.body[0], headless))
            gameOver();
    }

    void gameOver()
    {
        snake_.reset();
        food_.respawn(snake_.body);
        running_ = false;
        score_   = 0;
    }

    Snake snake_ {};
    Food  food_{snake_.body};
    bool  running_   {true};
    unsigned int   score_      {};
    bool  allowMove_  {};
};


// factory 
[[nodiscard]]
std::unique_ptr<Game> createGame()
{
    return std::make_unique<Game>();
}

// main 
int main()
{
    std::cout << "Starting the game...\n";

    constexpr int winSize { 2 * kOffset + kCellSize * kCellCount };
    InitWindow(winSize, winSize, "Retro Snake");
    SetTargetFPS(60);

    auto game {createGame()};

    while (!WindowShouldClose()) {
        if (eventTriggered(kTickRate)) {
            game->setAllowMove(true);
            game->update();
        }
        game->processInput();

        BeginDrawing();
        ClearBackground(kGreen);

        DrawRectangleLinesEx(
            Rectangle{
                static_cast<float>(kOffset) - 5,
                static_cast<float>(kOffset) - 5,
                static_cast<float>(kCellSize * kCellCount) + 10,
                static_cast<float>(kCellSize * kCellCount) + 10
            }, 5, kDarkGreen);

        DrawText("Retro Snake", kOffset - 5, 20, 40, kDarkGreen);
        DrawText(TextFormat("%i", game->score()),
                 kOffset - 5,
                 kOffset + kCellSize * kCellCount + 10,
                 40, kDarkGreen);

        game->draw();
        EndDrawing();
    }

    CloseWindow();
}