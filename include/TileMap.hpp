#pragma once

#include <raylib.h>

#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

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
            int mouseIndex = getIndexWorldPos(mousePos);
            if (indexValid(mouseIndex))
            {
                m_tileMap[mouseIndex].hovered = true;
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

            for (int y = tileTopLeft.y; y < tileBottomRight.y; ++y)
            {
                for (int x = tileTopLeft.x; x < tileBottomRight.x; ++x)
                {
                    if (!tilePosValid({(float)x, (float)y})) continue;
                    int i = getIndexTilePos({(float)x, (float)y});

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
            if (!tilePosValid(tilePos)) return -1;
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

        Tile* getTile(int index)
        {
            if (!indexValid(index)) return nullptr;
            return &m_tileMap[index];
        }

        int width() const {return m_mapWidth;}
        int height() const {return m_mapHeight;}

        bool tilePosValid(Vector2 tilePos) 
        {
            return tilePos.x >= 0 && tilePos.x < m_mapWidth &&
                   tilePos.y >= 0 && tilePos.y < m_mapHeight;
        }

        bool indexValid(int index) 
        {
            return index >= 0 && index < m_mapWidth * m_mapHeight;
        }
    };
}