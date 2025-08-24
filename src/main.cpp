#include <raylib.h>
#include <raymath.h>

#include "imgui.h"
#include "rlImGui.h"

#include <string>
#include <iostream>
#include <vector>
#include <deque>

#include "../include/Car.hpp"
#include "../include/MapManager.hpp"

void handleInput(const float dt, Car* car)
{
    car->input(dt);
}

void update(const float dt, Map::MapManager* mapManager, Car* car, Camera2D& cam)
{
    car->update(dt, mapManager);

    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), cam);
    int mouseIndex = mapManager->tileMap()->getIndexWorldPos(mousePos);

    if (mapManager->tileMap()->indexValid(mouseIndex))
    {
        Map::Tile* tile = mapManager->tileMap()->getTile(mouseIndex);
        if (tile && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
        {
            if (tile->type == Map::TileType::NONE) tile->type = Map::TileType::ROAD;
            else  tile->type = Map::TileType::NONE;
        }
    }

    mapManager->tileMap()->update(cam);
}

void render(Car* car, Map::MapManager* mapManager, Camera2D& cam)
{
    BeginDrawing();
    ClearBackground(GRAY);

    BeginMode2D(cam);
    mapManager->tileMap()->render(cam);
    car->render(mapManager);
    cam.target = car->getPos();
    EndMode2D();

    float boostBarX = GetScreenWidth() * (float)80/100;
    float boostBarY = GetScreenHeight() * (float)2/100;

    Vector2 boostBarFrameSize = {GetScreenWidth() * (float)18/100, 
        GetScreenHeight() * (float)5/100};
    Vector2 boostBarSize = {GetScreenWidth() * car->getBoostLevel() * (float)18/100 * (float)1/100, 
        GetScreenHeight() * (float)5/100};

    DrawRectangleLinesEx({boostBarX - 2.f, boostBarY - 2.f, boostBarFrameSize.x + 4.f, boostBarFrameSize.y + 4.f}, 2.f, BLACK);
    DrawRectangle(boostBarX, boostBarY, boostBarSize.x, boostBarSize.y, SKYBLUE);

    rlImGuiBegin();

    car->tuner();

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

    const int tileWidth = 64;
    const int tileHeight = 64;

    std::string selectedMapPath = "data/map.txt";

    Map::MapManager mapManager;
    mapManager.loadMap(selectedMapPath, tileWidth, tileHeight);

    const float trailTime = 0.001f;
    const size_t maxTrails = 100000;

    const float accelerationSpeed = 500.f;
    const float decelerationSpeed = 400.f;
    const float turnSpeed = 10.f;

    const float rollFriction = 0.03f;
    const float airFriction = 0.03f;
    const float grip = 5.f;

    const Vector2 startPos = {mapManager.tileMap()->width() * tileWidth * 0.5f, mapManager.tileMap()->height() * tileHeight * 0.5f};
    const Vector2 size = {30.f, 60.f};

    Car car(trailTime, maxTrails, accelerationSpeed, decelerationSpeed, 
            turnSpeed, rollFriction, airFriction, grip, startPos, size, &vehicleTex);

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
        update(dt, &mapManager, &car, cam);
        render(&car, &mapManager, cam);
    }

    // close game

    mapManager.saveMap(selectedMapPath);
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}