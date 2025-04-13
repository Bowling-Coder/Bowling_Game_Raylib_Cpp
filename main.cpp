#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int BALL_RADIUS = 15;
const int PIN_WIDTH = 10;
const int PIN_HEIGHT = 30;

struct Pin {
    Vector2 position;
    bool knockedDown;
};

void InitPins(vector<Pin> &pins) {
    pins.clear();
    float startX = SCREEN_WIDTH / 2.0f;
    float startY = 120;
    int pinCount = 4;

    for (int row = 0; row < 4; row++) {
        float rowStartX = startX - (pinCount - 1) * (PIN_WIDTH + 20) / 2.0f;

        for (int i = 0; i < pinCount; i++) {
            Pin pin;
            pin.position = { rowStartX + i * (PIN_WIDTH + 20), startY + row * (PIN_HEIGHT + 10) };
            pin.knockedDown = false;
            pins.push_back(pin);
        }
        pinCount--; // Decrease pin count per row to make triangle
    }
}

void ResetBall(int &ball_x, int &ball_y) {
    ball_x = SCREEN_WIDTH / 2;
    ball_y = SCREEN_HEIGHT - 50;
}

bool CheckCollision(Vector2 ballPos, const Pin& pin) {
    Rectangle pinRect = { pin.position.x, pin.position.y, (float)PIN_WIDTH, (float)PIN_HEIGHT };
    return !pin.knockedDown && CheckCollisionCircleRec(ballPos, BALL_RADIUS, pinRect);
}

int main() {
    srand(time(0));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bowling Game - Aiming Fixed on 2nd Roll");
    SetTargetFPS(60);

    int ball_x = SCREEN_WIDTH / 2;
    int ball_y = SCREEN_HEIGHT - 50;
    bool ballRolling = false;
    int score = 0;
    bool showScore = false;

    vector<Pin> pins;
    InitPins(pins);

    int rollCount = 0; // Track the number of rolls (1st or 2nd roll)

    while (!WindowShouldClose()) {
        // Handle aiming
        if (!ballRolling) {
            if (IsKeyDown(KEY_LEFT)) ball_x -= 5;
            if (IsKeyDown(KEY_RIGHT)) ball_x += 5;

            // Clamp to lane bounds
            int leftLimit = SCREEN_WIDTH / 4 + BALL_RADIUS;
            int rightLimit = SCREEN_WIDTH * 3 / 4 - BALL_RADIUS;
            ball_x = Clamp(ball_x, leftLimit, rightLimit);

            if (IsKeyPressed(KEY_ENTER)) {
                ballRolling = true;
                showScore = false;
                ball_y = SCREEN_HEIGHT - 50; // Always reset Y so it starts from bottom
            }
        }

        if (ballRolling) {
            ball_y -= 5;

            // Check collisions
            for (auto &pin : pins) {
                if (CheckCollision({ (float)ball_x, (float)ball_y }, pin)) {
                    pin.knockedDown = true;
                }
            }

            // Stop the ball at the top and reset after each roll
            if (ball_y <= 100) {
                ballRolling = false;
                showScore = true;
                score = count_if(pins.begin(), pins.end(), [](const Pin &p) { return p.knockedDown; });

                if (rollCount == 0) {
                    rollCount = 1; // First roll is complete
                    ball_y = SCREEN_HEIGHT - 50; // Reset just the Y position for second roll
                } else {
                    rollCount = 0; // Second roll complete
                    InitPins(pins); // Reinitialize pins
                    ResetBall(ball_x, ball_y); // Fully reset ball
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        // Draw lane
        DrawRectangle(SCREEN_WIDTH / 4, 80, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 100, DARKGRAY);

        // Draw ball
        DrawCircle(ball_x, ball_y, BALL_RADIUS, RED);

        // Draw pins
        for (auto &pin : pins) {
            if (!pin.knockedDown) {
                DrawRectangle(pin.position.x, pin.position.y, PIN_WIDTH, PIN_HEIGHT, WHITE);
            }
        }

        // UI
        DrawText("← → to aim | [ENTER] to roll", 10, 10, 20, RAYWHITE);
        DrawText(TextFormat("Pins remaining: %d",
            (int)count_if(pins.begin(), pins.end(), [](const Pin &p) { return !p.knockedDown; })), 10, 40, 20, RAYWHITE);

        if (showScore) {
            DrawText(TextFormat("Score this roll: %d", score), 10, 70, 20, YELLOW);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
