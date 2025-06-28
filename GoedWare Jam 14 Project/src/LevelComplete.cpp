#include "LevelComplete.h"
#include "Player.h"

LevelComplete::LevelComplete()
{
}

LevelComplete::~LevelComplete()
{
}

void LevelComplete::InitializeSprite(std::string childNode2_)
{
    document.parse<0>(barFile.data());

    rootNode = document.first_node("Bar");

    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("BarInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node(childNode2_.c_str()); fileNode2;
            fileNode2 = fileNode->next_sibling())
        {
            rectangle =
            {
                static_cast<float>(atof(fileNode2->first_attribute("posX")->value())),
                static_cast<float>(atof(fileNode2->first_attribute("posY")->value())),
                static_cast<float>(atof(fileNode2->first_attribute("recWidth")->value())),
                static_cast<float>(atof(fileNode2->first_attribute("recHeight")->value()))
            };
        }
    }
}

void LevelComplete::DrawSprite(Color color_)
{
    if (Player::Instance()->GetLightOn()) DrawRectangleRec(rectangle, color_);
}