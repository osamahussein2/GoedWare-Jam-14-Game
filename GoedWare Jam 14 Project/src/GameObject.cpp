#include "GameObject.h"

GameObject::GameObject() : spriteFile("XML files/GameObject.xml"), rootNode(), position(), rectangle(), texture(),
insideLight(false)
{

}

GameObject::~GameObject()
{
}

void GameObject::InitializeGameObject(std::string childNode2_)
{
    document.parse<0>(spriteFile.data());

    rootNode = document.first_node("GameObject");

    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("GameObjectInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node(childNode2_.c_str()); fileNode2;
            fileNode2 = fileNode->next_sibling())
        {
            texture = LoadTexture(fileNode2->first_attribute("spritePath")->value());

            rectangle.width = static_cast<float>(texture.width);
            rectangle.height = static_cast<float>(texture.height);
            rectangle.x = 0.0f;
            rectangle.y = 0.0f;

            position = Vector2{ static_cast<float>(atof(fileNode2->first_attribute("posX")->value())),
                static_cast<float>(atof(fileNode2->first_attribute("posY")->value()))};
        }
    }
}

void GameObject::DrawSprite(Vector2 center_, float radius_, Color color_, bool lightOn_)
{
    if (lightOn_ && center_.x - radius_ / 2.0f <= position.x + rectangle.width &&  center_.x + radius_ / 2.0f >= position.x &&
        center_.y - radius_ / 2.0f <= position.y + rectangle.height && center_.y + radius_ / 2.0f >= position.y)
    {
        if (insideLight != true) insideLight = true;
    }

    else
    {
        if (insideLight != false) insideLight = false;
    }

    if (insideLight) DrawTextureRec(texture, rectangle, position, color_);
}

void GameObject::UnloadSprite()
{
    UnloadTexture(texture);
}
