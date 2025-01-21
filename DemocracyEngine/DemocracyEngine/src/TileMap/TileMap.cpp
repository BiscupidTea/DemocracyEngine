#include "TileMap.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "../Animation/Frame.h"
#include "../Tools/tinyxml2.h"

using namespace tinyxml2;
using namespace std;

namespace DemoEngine_TileMap
{
    TileMap::TileMap(vec3 newPosition, vec3 newRotation, vec3 newScale,
                     const char* tileSetFile, const vector<const char*>& tileMapFiles,
                     const char* tileMapImage): Entity2D(newPosition, newRotation, newScale)
    {
        LoadTileSet(tileSetFile);
        LoadTileMaps(tileMapFiles);

        mapTileHeight = (mapPixelHeight / tilePixelHeight);
        mapTileWidth = (mapPixelWidth / tilePixelWidth);

        vertexSize = 36 * mapTileHeight * mapTileWidth;
        
        vector<float> vertexVector;

        //          {
        //             // positions		    // colors					// texture coords
        //             1, 1, 0,       1.0f, 1.0f, 1.0f, 1.0f,       uvX + uvWidth, uvY + uvHeight, // top right
        //             1, 0, 0,      1.0f, 1.0f, 1.0f, 1.0f,         uvX + uvWidth, uvY, // bottom right
        //             0, 0, 0,      1.0f, 1.0f, 1.0f, 1.0f,        uvX, uvY, // bottom left
        //             0, 1, 0,       1.0f, 1.0f, 1.0f, 1.0f,       uvX, uvY + uvHeight // top left 
        //         };

        for (int y = 0; y < mapTileHeight; y++)
        {
            for (int x = 0; x < mapTileWidth; x++)
            {
                // top right
                {
                    //position
                    vertexVector.push_back(static_cast<float>(1 + x));
                    vertexVector.push_back(static_cast<float>(1 - y));
                    vertexVector.push_back(0.0f);

                    //color
                    vertexVector.push_back(1.0f);
                    vertexVector.push_back(1.0f);
                    vertexVector.push_back(1.0f);
                    vertexVector.push_back(1.0f);

                    //UV
                    vertexVector.push_back(tileSet.at(tileMap.at(0).getTileId(x,y)).topRightUV.x);
                    vertexVector.push_back(1.0f);
                }
                // bottom right
                {
                    //position
                    vertexVector.push_back(static_cast<float>(1 + x));
                    vertexVector.push_back(static_cast<float>(0 - y));
                    vertexVector.push_back(0.0f);

                    //color
                    vertexVector.push_back(1.0f);
                    vertexVector.push_back(1.0f);
                    vertexVector.push_back(1.0f);
                    vertexVector.push_back(1.0f);

                    //UV
                    vertexVector.push_back(1.0f);
                    vertexVector.push_back(0.0f);
                }
                // bottom left
                {
                    //position
                    vertexVector.push_back(static_cast<float>(0 + x));
                    vertexVector.push_back(static_cast<float>(0 - y));
                    vertexVector.push_back(0.0f);

                    //color
                    vertexVector.push_back(1.0f);
                    vertexVector.push_back(1.0f);
                    vertexVector.push_back(1.0f);
                    vertexVector.push_back(1.0f);

                    //UV
                    vertexVector.push_back(0.0f);
                    vertexVector.push_back(0.0f);
                }
                // top left
                {
                    //position
                    vertexVector.push_back(static_cast<float>(0 + x));
                    vertexVector.push_back(static_cast<float>(1 - y));
                    vertexVector.push_back(0.0f);

                    //color
                    vertexVector.push_back(1.0f);
                    vertexVector.push_back(1.0f);
                    vertexVector.push_back(1.0f);
                    vertexVector.push_back(1.0f);

                    //UV
                    vertexVector.push_back(0.0f);
                    vertexVector.push_back(1.0f);
                }
            }
        }

        indexSize = 6 * mapTileHeight * mapTileWidth;
        vector<int> indicesVector;
        for (int i = 0; i < mapTileWidth * mapTileHeight; i++)
        {
            indicesVector.push_back(0 + (4 * i));
            indicesVector.push_back(1 + (4 * i));
            indicesVector.push_back(3 + (4 * i));
            indicesVector.push_back(1 + (4 * i));
            indicesVector.push_back(2 + (4 * i));
            indicesVector.push_back(3 + (4 * i));
        }

        // int indices[] = {
        //     0, 1, 3,
        //     1, 2, 3
        // };

        vertex = new float[vertexSize];
        std::copy(std::begin(vertexVector), std::end(vertexVector), vertex);

        indices = new int[indexSize];
        std::copy(std::begin(indicesVector), std::end(indicesVector), indices);

        Renderer::GetRender()->CreateSprite(VBO, VAO, EBO, vertex, indices, vertexSize, indexSize);
        Renderer::GetRender()->BindTexture(tileMapImage, tileMapTexture);
    }

    TileMap::~TileMap()
    {
        delete vertex;
        delete indices;
    }

    void TileMap::LoadTileMaps(const vector<const char*>& tileMapFiles)
    {
        layerCount = tileMapFiles.size();
        tileMap.clear();

        for (const auto& file : tileMapFiles)
        {
            TileMapLayer layerData = ReadSingleMap(file);

            tileMap.push_back(layerData);
        }

        for (int layerIndex = 0; layerIndex < tileMap.size(); ++layerIndex)
        {
            cout << "Layer " << layerIndex + 1 << ":\n";
            for (int y = 0; y < tileMap.at(layerIndex).y; ++y)
            {
                for (int x = 0; x < tileMap.at(layerIndex).x; ++x)
                {
                    cout << tileMap.at(layerIndex).getTileId(x, y) << " ";
                }
                cout << endl;
            }
            cout << "\n";
        }
    }

    void TileMap::LoadTileSet(const char* tileSetFile)
    {
        XMLDocument doc;
        if (doc.LoadFile(tileSetFile) != XML_SUCCESS)
        {
            cout << "Error loading file XML: " << tileSetFile << endl;
            return;
        }

        XMLElement* root = doc.FirstChildElement("tileset");
        if (!root)
        {
            cout << "Error: file is not a tileset file" << endl;
            return;
        }

        root->QueryIntAttribute("tilewidth", &tilePixelWidth);
        root->QueryIntAttribute("tileheight", &tilePixelHeight);

        root = doc.FirstChildElement("tileset")->FirstChildElement("image");
        root->QueryIntAttribute("width", &mapPixelWidth);
        root->QueryIntAttribute("height", &mapPixelHeight);

        vector<pair<vec2, vec2>> tileSetUV = CalculateUVCoordsInMap(mapPixelHeight, mapPixelWidth, tilePixelHeight,
                                                                    tilePixelWidth);

        root = doc.FirstChildElement("tileset");

        XMLElement* tileElement = root->FirstChildElement("tile");
        while (tileElement)
        {
            int tileId = tileElement->IntAttribute("id");
            bool hasCollision = false;

            XMLElement* properties = tileElement->FirstChildElement("properties");
            if (properties)
            {
                XMLElement* property = properties->FirstChildElement("property");
                while (property)
                {
                    const char* name = property->Attribute("name");
                    if (name && strcmp(name, "Obstacle") == 0)
                    {
                        const char* value = property->Attribute("value");
                        if (value && strcmp(value, "true") == 0)
                        {
                            hasCollision = true;
                            break;
                        }
                    }
                    property = property->NextSiblingElement("property");
                }
            }

            tileSet.push_back(Tile(tileId, hasCollision, tileSetUV.at(tileId).first, tileSetUV.at(tileId).second));
            tileElement = tileElement->NextSiblingElement("tile");
        }

        for (Tile currentTile : tileSet)
        {
            cout << "Tile loaded = ID: " << currentTile.id
                << " - hasCollision: " << currentTile.hasCollision
                << " - UV: (" << currentTile.topRightUV.x << "," << currentTile.topRightUV.x << ")"
                << " - UV: (" << currentTile.topLeftUV.y << "," << currentTile.topLeftUV.y << ")"
                << " - UV: (" << currentTile.topLeftUV.y << "," << currentTile.topLeftUV.y << ")"
                << " - UV: (" << currentTile.topLeftUV.y << "," << currentTile.topLeftUV.y << ")"
                << endl;
        }

        cout << "Loaded " << tileSet.size() << " tiles" << endl;
        cout << endl;
    }

    void TileMap::Draw()
    {
        Renderer::GetRender()->DrawTexture(VAO, indexSize, color, model, tileMapTexture);
    }

    bool TileMap::hasCollision(int layer, Entity2D entity)
    {
        return true;
    }

    bool TileMap::hasCollision(int layer, int x, int y, int width, int height)
    {
        return true;
    }

    TileMapLayer TileMap::ReadSingleMap(const char* filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cout << "Error to open file: " << filename << endl;
            return TileMapLayer(0, 0);
        }

        vector<vector<int>> tempLayer;
        string line;

        while (getline(file, line))
        {
            vector<int> row;
            stringstream ss(line);
            string cell;
            while (getline(ss, cell, ','))
            {
                row.push_back(stoi(cell));
            }
            tempLayer.push_back(row);
        }

        file.close();

        int width = tempLayer[0].size();
        int height = tempLayer.size();

        TileMapLayer mapData(width, height);

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                mapData.setTile(x, y, tempLayer[y][x]);
            }
        }

        return mapData;
    }

    vector<pair<vec2, vec2>> TileMap::CalculateUVCoordsInMap(int totalHeight, int totalWidth, int tileHeight,
                                                             int tileWidth)
    {
        vector<pair<vec2, vec2>> uvCoordsList;

        int numHeight = totalHeight / tileHeight;
        int numWidth = totalWidth / tileWidth;

        for (int i = 0; i < numHeight; ++i)
        {
            for (int j = 0; j < numWidth; ++j)
            {
                vec2 uv1 = {
                    static_cast<float>(j * tileWidth) / totalWidth, static_cast<float>(i * tileHeight) / totalHeight
                };
                vec2 uv2 = {
                    static_cast<float>((j + 1) * tileWidth) / totalWidth,
                    static_cast<float>((i + 1) * tileHeight) / totalHeight
                };

                uvCoordsList.push_back({uv1, uv2});
            }
        }

        return uvCoordsList;
    }
}
