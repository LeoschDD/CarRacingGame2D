#pragma once

#include "Map.hpp"

namespace Map
{
    class MapManager
    {
    private:
        std::unique_ptr<TileMap> m_currentMap;

    public:
        void createMap(int tileWidth, int tileHeight, int mapWidth, int mapHeight)
        {
            m_currentMap = std::make_unique<TileMap>(tileWidth, tileHeight, mapWidth, mapHeight);
        }

        void loadMap(std::string path, int tileWidth, int tileHeight)
        {
            std::vector<int> tileMap;

            std::ifstream file(path);
            if(!file) throw std::runtime_error("Error in loadMap(): File " + path + " couldnt open!");

            std::string line;
            int x;

            int mapWidth{};
            int mapHeight{};

            file >> mapWidth >> mapHeight;

            while (std::getline(file, line))
            {
                std::stringstream iss(line);
                while (iss >> x) tileMap.push_back(x);
            }

            if (tileMap.size() != (size_t)(mapWidth * mapHeight)) 
            {
                throw std::runtime_error("Error in loadMap(): Set map size doesnt equal actual map size!");
            }

            m_currentMap = std::make_unique<TileMap>(tileWidth, tileHeight, mapWidth, mapHeight);
            m_currentMap->loadMap(tileMap, mapWidth, mapHeight);
        }

        void saveMap(std::string path)
        {   
            std::vector<int> tileMap = map()->saveMap();

            std::ofstream file(path);
            if (!file) throw std::runtime_error("Error in saveMap(): File " + path + " couldnt open!");

            file << map()->width() << " " << map()->height() << std::endl;

            for (float y = 0; y < map()->height(); ++y)
            {
                for (float x = 0; x < map()->width(); ++x)
                {
                    int i = map()->getIndexTilePos({x, y}); 
                    if (!map()->indexValid(i)) 
                    {
                        throw std::runtime_error("Error in saveMap(): Set map size doesnt equal actual map size!");
                    }
                    file << tileMap[i] << " ";
                }
                file << std::endl;
            }
        }

        TileMap* map() {return m_currentMap.get();}
    };  
}

