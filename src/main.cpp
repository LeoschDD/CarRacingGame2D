#include <raylib.h>
#include <raymath.h>

#include "imgui.h"
#include "rlImGui.h"

#include <string>
#include <iostream>
#include <vector>
#include <deque>

#include "../include/Car.hpp"
#include "../include/World.hpp"

void handleInput(const float dt, Car* car)
{
    car->input();
}

void update(const float dt, Car* car)
{
    car->update(dt);
}

void render(Car* car, World* world, Camera2D& cam)
{
    BeginDrawing();
    ClearBackground(GRAY);

    BeginMode2D(cam);
    world->render(cam);
    car->render();
    cam.target = car->getPos();
    EndMode2D();

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

    Image vehicleImg = LoadImage("assets/Spritesheets/spritesheet_vehicles.png");
    Texture2D vehicleTex = LoadTextureFromImage(vehicleImg);

    const uint16_t tileWidth = 64;
    const uint16_t tileHeight = 64;

    const size_t mapWidth = 2000;    
    const size_t mapHeight = 2000;

    World world(tileWidth, tileHeight, mapWidth, mapHeight);

    const float trailTime = 0.03f;
    const size_t maxTrails = 2000;

    const float maxVelocity = 10000.f;
    const float minVelocity = -500.f;

    const float accelerationSpeed = 1000.f;
    const float decelerationSpeed = 400.f;
    const float turnSpeed = 10.f;

    const float rollFriction = 0.001f;
    const float airFriction = 0.001f;
    const float grip = 5.f;

    const Vector2 startPos = {mapWidth * tileWidth * 0.5f, mapHeight * tileHeight * 0.5f};
    const Vector2 size = {30.f, 60.f};

    Car car(trailTime, maxTrails, maxVelocity, minVelocity, 
            accelerationSpeed, decelerationSpeed, turnSpeed, 
            rollFriction, airFriction, grip, startPos, size, &vehicleTex);

    Camera2D cam;
    cam.offset = {(float)screenWidth/2, (float)screenHeight/2};
    cam.rotation = 0.f;
    cam.target = startPos;
    cam.zoom = 1.f;
    
    // game loop

    while (!WindowShouldClose())
    {   
        const float dt = GetFrameTime();

        handleInput(dt, &car);
        update(dt, &car);
        render(&car, &world, cam);
    }

    // close game

    rlImGuiShutdown();
    CloseWindow();

    return 0;
}