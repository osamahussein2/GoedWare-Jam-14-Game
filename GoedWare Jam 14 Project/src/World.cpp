#include "World.h"

std::shared_ptr<World> World::worldInstance = nullptr;

World::World()
{
}

World::~World()
{
}

std::shared_ptr<World> World::Instance()
{
    if (worldInstance == nullptr)
    {
        worldInstance = std::make_shared<World>();
        return worldInstance;
    }

    return worldInstance;
}

void World::InitializeGameObject(const char* childNode2_)
{
    document.parse<0>(spriteFile.data());

    rootNode = document.first_node("GameObject");

    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("GameObjectInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node(childNode2_); fileNode2;
            fileNode2 = fileNode->next_sibling())
        {
            texture = LoadTexture(fileNode2->first_attribute("spritePath")->value());

            rectangle.width = static_cast<float>(texture.width);
            rectangle.height = static_cast<float>(texture.height);
            rectangle.x = 0.0f;
            rectangle.y = 0.0f;

            position = Vector2{ static_cast<float>(atof(fileNode2->first_attribute("posX")->value())),
                static_cast<float>(atof(fileNode2->first_attribute("posY")->value())) };
        }
    }
}

void World::DrawWorld(bool lightOn_, Color color_)
{
    if (lightOn_) DrawTextureRec(texture, rectangle, position, color_);
}

void World::UnloadWorld()
{
    UnloadTexture(texture);
}
