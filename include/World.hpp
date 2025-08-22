#pragma once 

#include <raylib.h>
#include <raymath.h>

#include <vector>
#include <cstdint>

class World
{
private:
    uint16_t m_tileWidth;
    uint16_t m_tileHeight;

    size_t m_mapWidth;
    size_t m_mapHeight;

    std::vector<size_t> m_tileMap;

public:
    World(uint16_t tileWidth, 
        uint16_t tileHeight, 
        size_t mapWidth, 
        size_t mapHeight)
        : m_tileWidth(tileWidth)
        , m_tileHeight(tileHeight)
        , m_mapWidth(mapWidth)
        , m_mapHeight(mapHeight)
    {
        m_tileMap = std::vector<size_t> (m_mapWidth * m_mapHeight, 0);
    }

    World(uint16_t tileWidth, 
        uint16_t tileHeight, 
        size_t mapWidth, 
        size_t mapHeight, 
        std::vector<size_t> tileMap)
        : m_tileWidth(tileWidth)
        , m_tileHeight(tileHeight)
        , m_mapWidth(mapWidth)
        , m_mapHeight(mapHeight)
        , m_tileMap(tileMap)

    {}

    ~World() = default;

    void render(Camera2D& cam)
    {
        Vector2 topLeft = GetScreenToWorld2D({0.f, 0.f}, cam);
        Vector2 bottomRight = {topLeft.x + GetScreenWidth(), topLeft.y + GetScreenHeight()};

        Vector2 tileTopLeft = {floor(topLeft.x / m_tileWidth), floor(topLeft.y / m_tileHeight)};
        Vector2 tileBottomRight = {floor(bottomRight.x / m_tileWidth) + 1.f, floor(bottomRight.y / m_tileHeight) + 1.f};

        for (int x = tileTopLeft.x; x < tileBottomRight.x; x++)
        {
            for (int y = tileTopLeft.y; y < tileBottomRight.y; y++)
            {
                size_t i = getIndexFromTile({(float)x, (float)y});
                Vector2 pos = getPos(i);

                Color color = (((x + y) & 1) == 0) ? Color{50, 50, 50, 200} : Color{10, 10, 10, 100};

                DrawRectangle(pos.x, pos.y, m_tileWidth, m_tileHeight, color);
            }
        }
    }

    size_t getIndexFromPos(Vector2 pos) 
    {
        return (floor((size_t)pos.x / m_tileWidth) + floor((size_t)pos.y / m_tileHeight) * m_mapWidth);
    }

    size_t getIndexFromTile(Vector2 tilePos) 
    {
        return ((size_t)tilePos.x + (size_t)tilePos.y * m_mapWidth);
    }

    Vector2 getPos(size_t index)
    {
        Vector2 tilePos = {(float)(index % m_mapWidth), (float)(index / m_mapWidth)};
        return {tilePos.x * m_tileWidth, tilePos.y * m_tileHeight};
    }
};