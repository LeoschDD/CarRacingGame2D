#include <raylib.h>
#include <raymath.h>

#include "imgui.h"
#include "rlImGui.h"

#include <string>
#include <iostream>
#include <vector>
#include <deque>

#include "../include/Car.hpp"

void handleInput(const float dt, Car* car)
{
    car->input();
}

void update(const float dt, Car* car)
{
    car->update(dt);
}

void render(Car* car)
{
    BeginDrawing();
    ClearBackground(GRAY);

    rlImGuiBegin();

    static bool open = true;
    static ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
    ImGui::Begin("Car-Info", &open, flags);

    ImGui::SetWindowPos({0, 0});
    ImGui::SetWindowSize({200, 1080});

    ImGui::Text("Position x: %2.f", car->getPos().x);
    ImGui::Text("Position y: %2.f", car->getPos().y);

    ImGui::Text("Velocity x: %2.f", car->getVel().x);
    ImGui::Text("Velocity y: %2.f", car->getVel().y);

    ImGui::Text("Throttle: %2.f", car->getThrottle());

    ImGui::Text("Rotation: %2.f", car->getRotation());

    ImGui::End();

    rlImGuiEnd();

    car->render();

    EndDrawing();
}

int main(int argc, char** argv)
{
    // set constant values

    int screenWidth = 1920;
    int screenHeight = 1080;    

    const char* title = "Racing Game";

    // init game

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, title);
    SetTargetFPS(240);
    rlImGuiSetup(true);

    // create car

    const float trailTime = 0.03f;
    const size_t maxTrails = 2000;

    const float maxVelocity = 800.f;
    const float minVelocity = -300.f;

    const float accelerationSpeed = 500.f;
    const float decelerationSpeed = 300.f;
    const float turnSpeed = 1.6f;

    const float rollFriction = 0.001f;
    const float airFriction = 0.001f;
    const float grip = 6.f;

    const Vector2 startPos = {500.f, 500.f};
    const Vector2 size = {30.f, 60.f};

    const Color color = ORANGE;

    Car car(trailTime, maxTrails, maxVelocity, minVelocity, 
            accelerationSpeed, decelerationSpeed, turnSpeed, 
            rollFriction, airFriction, grip, startPos, size, color);

    // game loop

    while (!WindowShouldClose())
    {   
        const float dt = GetFrameTime();

        handleInput(dt, &car);
        update(dt, &car);
        render(&car);
    }

    // close game

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}