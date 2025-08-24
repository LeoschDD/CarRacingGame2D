#pragma once

#include "TileMap.hpp"
#include "CollisionMap.hpp"

namespace Map
{
    class MapManager
    {
    private:
        std::unique_ptr<TileMap> m_currentTileMap;
        std::unique_ptr<CollisionMap> m_currentCollisionMap;

    public:
        void createMap(int tileWidth, int tileHeight, int mapWidth, int mapHeight)
        {
            m_currentTileMap = std::make_unique<TileMap>(tileWidth, tileHeight, mapWidth, mapHeight);
            m_currentCollisionMap = std::make_unique<CollisionMap>(tileWidth, tileHeight, mapWidth, mapHeight);
        }

        void loadMap(std::string path, int tileWidth, int tileHeight)
        {
            std::vector<int> tileMap;
            std::vector<int> collisionMap;

            std::ifstream file(path);
            if(!file) throw std::runtime_error("loadMap: file " + path + " couldnt open!");

            std::string line;
            int x;

            int mapWidth{};
            int mapHeight{};

            if (!(file >> mapWidth >> mapHeight)) 
            {
                throw std::runtime_error("loadMap: missing map size in header!");
            }
            
            tileMap.reserve(mapWidth * mapHeight);

            while (std::getline(file, line))
            {
                if (line.empty()) continue; 

                std::stringstream sstream(line);
                std::string first;

                if (!(sstream >> first)) continue; 

                if (first == "tileMap") continue;
                if (first == "collisionMap") break;
                
                std::istringstream (first) >> x;
                tileMap.push_back(x);
                while(sstream >> x) tileMap.push_back(x);    
            }

            while (file >> x) collisionMap.push_back(x);

            if (tileMap.size() != (size_t)(mapWidth * mapHeight)) 
            {
                throw std::runtime_error("loadTileMap: set map size doesnt equal actual map size!");
            }

            if (collisionMap.size() != (size_t)(mapWidth * mapHeight)) 
            {
                throw std::runtime_error("loadCollisionMap: set map size doesnt equal actual map size!");
            }

            m_currentTileMap = std::make_unique<TileMap>(tileWidth, tileHeight, mapWidth, mapHeight);
            m_currentTileMap->loadMap(tileMap, mapWidth, mapHeight);

            m_currentCollisionMap = std::make_unique<CollisionMap>(tileWidth, tileHeight, mapWidth, mapHeight);
            m_currentCollisionMap->loadMap(collisionMap, mapWidth, mapHeight);
        }

        void saveMap(std::string path)
        {   
            if (!m_currentTileMap) throw std::runtime_error("saveMap: tried to save without an active map!");

            std::vector<int> tileMap = m_currentTileMap->saveMap();
            std::vector<int> collisionMap = m_currentCollisionMap->saveMap();

            std::ofstream file(path);
            if (!file) throw std::runtime_error("saveMap: File " + path + " couldnt open!");

            file << m_currentTileMap->width() << " " << m_currentTileMap->height() << std::endl;

            file << "tileMap" << std::endl;

            for (int y = 0; y < m_currentTileMap->height(); ++y)
            {
                for (int x = 0; x < m_currentTileMap->width(); ++x)
                {
                    int i = m_currentTileMap->getIndexTilePos({(float)x, (float)y}); 
                    if (!m_currentTileMap->indexValid(i)) 
                    {
                        throw std::runtime_error("saveTileMap: set map size doesnt equal actual map size!");
                    }
                    file << tileMap[i] << " ";
                }
                file << std::endl;
            }

            file << "collisionMap" << std::endl;

            for (int y = 0; y < m_currentCollisionMap->height(); ++y)
            {
                for (int x = 0; x < m_currentCollisionMap->width(); ++x)
                {
                    int i = m_currentCollisionMap->getIndexRectPos({(float)x, (float)y}); 
                    if (!m_currentCollisionMap->indexValid(i)) 
                    {
                        throw std::runtime_error("saveCollisionMap: set map size doesnt equal actual map size!");
                    }
                    file << collisionMap[i] << " ";
                }
                file << std::endl;
            }
        }

        TileMap* tileMap() const {return m_currentTileMap.get();}
        CollisionMap* collisionMap() const {return m_currentCollisionMap.get();}
    };
}