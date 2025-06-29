#include "Branch.h"
#include "Player.h"
#include "Window.h"

Branch::Branch() : runningTime(), totalFramesX(), xFrame(), playerSteppedOnBranch()
{
}

Branch::~Branch()
{
}

void Branch::InitializeGameObject(std::string childNode2_)
{
    document.parse<0>(spriteFile.data());

    rootNode = document.first_node("GameObject");

    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("GameObjectInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node(childNode2_.c_str()); fileNode2;
            fileNode2 = fileNode->next_sibling())
        {
            totalFramesX = atoi(fileNode2->first_attribute("totalFramesX")->value());

            texture = LoadTexture(fileNode2->first_attribute("spritePath")->value());

            texture.width *= atof(fileNode2->first_attribute("scaleMultiplier")->value());
            texture.height *= atof(fileNode2->first_attribute("scaleMultiplier")->value());

            rectangle.width = static_cast<float>(texture.width / totalFramesX);
            rectangle.height = static_cast<float>(texture.height);
            rectangle.x = 0.0f;
            rectangle.y = 0.0f;

            position = Vector2{ static_cast<float>(atof(fileNode2->first_attribute("posX")->value())),
                static_cast<float>(atof(fileNode2->first_attribute("posY")->value())) };
        }
    }
}

void Branch::DrawSprite(Color color_)
{
    if (runningTime < updateTime && playerSteppedOnBranch && xFrame < totalFramesX)
        runningTime += Window::Instance()->GetDeltaTime();

    else if (runningTime >= updateTime && playerSteppedOnBranch && xFrame < totalFramesX)
    {
        runningTime = 0.0f; // Reset running time

        // Update animation frame
        rectangle.x = rectangle.width * xFrame;
        xFrame++;
    }

    if (playerSteppedOnBranch)
    {
        Player::Instance()->SetPlayerNoiseVelocity(50.0f);
        playerSteppedOnBranch = false;
    }

    // If player has collided with the puddle and the bool is false, change the bool to true to trigger puddle animation
    if (Player::Instance()->GetPosition().x + Player::Instance()->GetRectangle().width >= position.x &&
        Player::Instance()->GetPosition().x <= position.x + rectangle.width &&
        Player::Instance()->GetPosition().y + Player::Instance()->GetRectangle().height >= position.y &&
        Player::Instance()->GetPosition().y <= position.y + rectangle.height && !playerSteppedOnBranch ||
        Player::Instance()->CheckForGameObjectCollision(position, rectangle) && !playerSteppedOnBranch)
    {
        playerSteppedOnBranch = true;
    }

    // Render puddle when player's light is on
    if (Player::Instance()->GetLightOn()) DrawTextureRec(texture, rectangle, position, color_);
}

void Branch::UnloadSprite()
{
	UnloadTexture(texture);
}

void Branch::ResetBranch(std::string childNode2_)
{
    if (runningTime != 0.0f) runningTime = 0.0f;
    if (xFrame != 0) xFrame = 0;
    if (playerSteppedOnBranch != false) playerSteppedOnBranch = false;

    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("GameObjectInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node(childNode2_.c_str()); fileNode2;
            fileNode2 = fileNode->next_sibling())
        {
            totalFramesX = atoi(fileNode2->first_attribute("totalFramesX")->value());

            texture = LoadTexture(fileNode2->first_attribute("spritePath")->value());

            texture.width *= atof(fileNode2->first_attribute("scaleMultiplier")->value());
            texture.height *= atof(fileNode2->first_attribute("scaleMultiplier")->value());

            rectangle.width = static_cast<float>(texture.width / totalFramesX);
            rectangle.height = static_cast<float>(texture.height);
            rectangle.x = 0.0f;
            rectangle.y = 0.0f;

            position = Vector2{ static_cast<float>(atof(fileNode2->first_attribute("posX")->value())),
                static_cast<float>(atof(fileNode2->first_attribute("posY")->value())) };
        }
    }
}
