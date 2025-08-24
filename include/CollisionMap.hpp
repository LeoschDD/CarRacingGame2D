#pragma once

#include <raylib.h>

#include <optional>
#include <vector>

namespace Map
{
    class CollisionMap
    {
    private:
        int m_rectWidth;
        int m_rectHeight;

        int m_mapWidth;
        int m_mapHeight;
        
        std::vector<bool> m_collisionMap;

    public:
        CollisionMap(int rectWidth, int rectHeight, int mapWidth, int mapHeight)
            : m_rectWidth(rectWidth)
            , m_rectHeight(rectHeight)
            , m_mapWidth(mapWidth)
            , m_mapHeight(mapHeight)
        {
            m_collisionMap = std::vector<bool> (m_mapWidth * m_mapHeight, 0);
        }

        void loadMap(std::vector<int> collisionMap, int mapWidth, int mapHeight)
        {
            m_mapWidth = mapWidth;
            m_mapHeight = mapHeight;

            m_collisionMap = std::vector<bool> (m_mapWidth * m_mapHeight, 0);

            for (int i = 0; i < m_mapWidth * m_mapHeight; ++i)
            {
                m_collisionMap[i] = static_cast<bool>(collisionMap[i]);
            }
        }

        std::vector<int> saveMap()
        {
            std::vector<int> tileMap (m_mapWidth * m_mapHeight, 0);

            for (int i = 0; i < m_mapWidth * m_mapHeight; ++i)
            {
                tileMap[i] = m_collisionMap[i] ? 1 : 0;
            }

            return tileMap;
        }

        int getIndexRectPos(Vector2 rectPos)
        {
            if (!rectPosValid(rectPos)) return -1;
            return (int)rectPos.y * m_mapWidth + (int)rectPos.x;
        }

        int getIndexWorldPos(Vector2 worldPos)
        {
            Vector2 rectPos = getRectPos(worldPos);
            return getIndexRectPos(rectPos);
        }
        
        Vector2 getRectPos(Vector2 worldPos)
        {
            return {floorf(worldPos.x / (float)m_rectWidth), 
                    floorf(worldPos.y / (float)m_rectHeight)};
        }

        Vector2 getRectPos(int index)
        {
            return {(float)(index % m_mapWidth), 
                    (float)(index / m_mapWidth)};
        }

        Vector2 getWorldPos(Vector2 rectPos)
        {
            return {rectPos.x * m_rectWidth, 
                    rectPos.y * m_rectHeight};
        }

        Vector2 getWorldPos(int index)
        {
            Vector2 rectPos = getRectPos(index);
            return getWorldPos(rectPos);
        }

        std::optional<Rectangle> getRect(int index)
        {
            if (!indexValid(index) || m_collisionMap[index] == 0) 
            {
                return std::nullopt;
            }

            Vector2 pos = getWorldPos(index);
            return Rectangle{pos.x, pos.y, (float)m_rectWidth, (float)m_rectHeight};
        }

        void setCollision(int index, bool value)
        {
            if (!indexValid(index)) return;
            m_collisionMap[index] = value;
        }

        int width() const {return m_mapWidth;}
        int height() const {return m_mapHeight;}

        bool rectPosValid(Vector2 rectPos) 
        {
            return rectPos.x >= 0 && rectPos.x < m_mapWidth &&
                   rectPos.y >= 0 && rectPos.y < m_mapHeight;
        }

        bool indexValid(int index) 
        {
            return index >= 0 && index < m_mapWidth * m_mapHeight;
        }
    };      
}