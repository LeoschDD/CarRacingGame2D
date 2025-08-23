#pragma once

#include <raylib.h>
#include <vector>

namespace Map
{   
    enum class TileType
    {
        NONE = 0,
        ROAD = 1
    };

    struct Tile
    {
        TileType type;
        Vector2 worldPos;
        bool hovered;
        bool selected;
    };  

    class TileMap
    {
    private:
        int m_tileWidth;
        int m_tileHeight;

        int m_mapWidth;
        int m_mapHeight;

        std::vector<Tile> m_tileMap;

    public:
        TileMap(int tileWidth, int tileHeight, int mapWidth, int mapHeight)
            : m_tileWidth(tileWidth)
            , m_tileHeight(tileHeight)
            , m_mapWidth(mapWidth)
            , m_mapHeight(mapHeight)
        {
            m_tileMap = std::vector<Tile> (m_mapWidth * m_mapHeight, Tile());

            for (int i = 0; i < m_mapWidth * m_mapHeight; ++i)
            {
                Tile tile{};
                tile.worldPos = getWorldPos(i);
                tile.type = TileType::NONE;
                tile.hovered = false;
                tile.selected = false;
                m_tileMap[i] = tile;
            }
        }

        ~TileMap() = default;

        // overwrite current tilemap with new tilemap

        void loadMap(std::vector<int> tileMap, int mapWidth, int mapHeight)
        {
            m_mapWidth = mapWidth;
            m_mapHeight = mapHeight;

            m_tileMap.resize(m_mapWidth * m_mapHeight);

            for (int i = 0; i < m_mapWidth * m_mapHeight; ++i)
            {
                Tile tile{};
                tile.worldPos = getWorldPos(i);
                tile.type = static_cast<TileType>(tileMap[i]);
                tile.hovered = false;
                tile.selected = false;
                m_tileMap[i] = tile;
            }
        }

        // save tilemap as vector of int, wich represent tiletype

        std::vector<int> saveMap()
        {
            std::vector<int> tileMap (m_mapWidth * m_mapHeight, 0);

            for (int i = 0; i < m_mapWidth * m_mapHeight; ++i)
            {
                int typeIndex = static_cast<int>(m_tileMap[i].type);
                tileMap[i] = typeIndex;
            }

            return tileMap;
        }

        // update

        void update(Camera2D& cam)
        {
            for (int i = 0; i < m_mapWidth * m_mapHeight; ++i)
            {
                Tile& tile = m_tileMap[i];
                tile.hovered = false;
            }

            Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), cam);
            int mouseTile = getIndexWorldPos(mousePos);
            if (mouseTile >= 0 && mouseTile < m_mapWidth * m_mapHeight)
            {
                m_tileMap[mouseTile].hovered = true;
            }
        }

        // render

        void render(Camera2D& cam)
        {
            Vector2 topLeftScreen = {0.f, 0.f};
            Vector2 topLeft = GetScreenToWorld2D(topLeftScreen, cam);

            Vector2 bottomRightScreen = {(float)GetScreenWidth(), (float)GetScreenHeight()};
            Vector2 bottomRight = GetScreenToWorld2D(bottomRightScreen, cam);

            Vector2 tileTopLeft = getTilePos(topLeft);
            Vector2 tileBottomRight = getTilePos(bottomRight);

            tileBottomRight.x += 1.f;
            tileBottomRight.y += 1.f;

            for (float x = tileTopLeft.x; x < tileBottomRight.x; ++x)
            {
                for (float y = tileTopLeft.y; y < tileBottomRight.y; ++y)
                {
                    int i = getIndexTilePos({x, y});
                    if (i < 0 || i >= m_mapWidth * m_mapHeight) continue;

                    Tile& tile = m_tileMap[i];

                    switch (tile.type)
                    {
                    case TileType::NONE:
                        DrawRectangleV(tile.worldPos, {(float)m_tileWidth, (float)m_tileHeight}, LIGHTGRAY);
                        break;
                    
                    case TileType::ROAD:
                        DrawRectangleV(tile.worldPos, {(float)m_tileWidth, (float)m_tileHeight}, DARKGRAY);
                        break;
                    }

                    if (tile.hovered) 
                    {
                        DrawRectangleV(tile.worldPos, {(float)m_tileWidth, (float)m_tileHeight}, ORANGE);
                    }
                }
            }
        }

        // helper functions

        int getIndexTilePos(Vector2 tilePos)
        {
            return (int)tilePos.y * m_mapWidth + (int)tilePos.x;
        }

        int getIndexWorldPos(Vector2 worldPos)
        {
            Vector2 tilePos = getTilePos(worldPos);
            return getIndexTilePos(tilePos);
        }
        
        Vector2 getTilePos(Vector2 worldPos)
        {
            return {floorf(worldPos.x / (float)m_tileWidth), 
                    floorf(worldPos.y / (float)m_tileHeight)};
        }

        Vector2 getTilePos(int index)
        {
            return {(float)(index % m_mapWidth), 
                    (float)(index / m_mapWidth)};
        }

        Vector2 getWorldPos(Vector2 tilePos)
        {
            return {tilePos.x * m_tileWidth, 
                    tilePos.y * m_tileHeight};
        }

        Vector2 getWorldPos(int index)
        {
            Vector2 tilePos = getTilePos(index);
            return getWorldPos(tilePos);
        }
    };
}