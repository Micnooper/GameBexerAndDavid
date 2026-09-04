#include "raylib.h"
#include <cmath>

struct Paddle {
    Rectangle rect;
    float speed;
    Color color;
};

struct Ball {
    Vector2 pos;
    float radius;
    float speedX;
    float speedY;
    Color color;
};

int main() {
    int screenWidth = 1920;
    int screenHeight = 1080;
    
    InitWindow(screenWidth, screenHeight, "Pong for MISP");
    SetTargetFPS(240);

    Paddle p1 = { { 120, 540, 15, 150 }, 6.0f, RED };

    Paddle p2 = { { 1800, 540, 15, 150 }, 6.0f, BLUE };

    Ball ball = { { 960, 540 }, 8, 4.0f, 4.0f, WHITE };

    int score1 = 0;
    int score2 = 0;

    while (!WindowShouldClose()) {
         
        if (IsKeyDown(KEY_W) && p1.rect.y > 0) p1.rect.y -= p1.speed;
        if (IsKeyDown(KEY_S) && p1.rect.y + p1.rect.height < screenHeight) p1.rect.y += p1.speed;
        
        if (IsKeyDown(KEY_UP) && p2.rect.y > 0) p2.rect.y -= p2.speed;
        if (IsKeyDown(KEY_DOWN) && p2.rect.y + p2.rect.height < screenHeight) p2.rect.y += p2.speed;

        ball.pos.x += ball.speedX;
        ball.pos.y += ball.speedY;

        if (ball.pos.y - ball.radius <= 0 || ball.pos.y + ball.radius >= screenHeight) {
            ball.speedY = -ball.speedY;
        }

        if (CheckCollisionCircleRec(ball.pos, ball.radius, p1.rect)) {
            ball.speedX = -ball.speedX;
            float relativeY = (ball.pos.y - (p1.rect.y + p1.rect.height / 2)) / (p1.rect.height / 2);
            ball.speedY = relativeY * 3.0f;
        }

        if (CheckCollisionCircleRec(ball.pos, ball.radius, p2.rect)) {
            ball.speedX = -ball.speedX;
            float relativeY = (ball.pos.y - (p2.rect.y + p2.rect.height / 2)) / (p2.rect.height / 2);
            ball.speedY = relativeY * 3.0f;
        }

        if (ball.pos.x < 0) {
            score2++;
            ball.pos = { 960, 540 };
            ball.speedX = 3.5f;
            ball.speedY = 3.5f;
        }
        if (ball.pos.x > screenWidth) {
            score1++;
            ball.pos = { 960, 540 };
            ball.speedX = -3.5f;
            ball.speedY = 3.5f;
        }

        BeginDrawing();
        ClearBackground(BLACK);
    
        for (int i = 0; i < screenHeight; i += 20) {
            DrawRectangle(960, i, 4, 10, DARKGRAY);
        }

        DrawRectangleRec(p1.rect, p1.color);
        DrawRectangleRec(p2.rect, p2.color);
        DrawCircleV(ball.pos, ball.radius, ball.color);

        DrawText(TextFormat("%d", score1), 909, 20, 30, WHITE);
        DrawText(TextFormat("%d", score2), 1000, 20, 30, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}