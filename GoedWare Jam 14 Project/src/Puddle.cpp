#include "Puddle.h"
#include "Player.h"
#include "Window.h"

Puddle::Puddle() : runningTime(), totalFramesX(), xFrame(), playerSteppedOnPuddle(), steppedOnPuddleTime()
{
}

Puddle::~Puddle()
{
}

void Puddle::InitializeGameObject(std::string childNode2_)
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

void Puddle::DrawSprite(Color color_)
{
    if (runningTime < updateTime && playerSteppedOnPuddle && xFrame < totalFramesX) 
        runningTime += Window::Instance()->GetDeltaTime();

    else if (runningTime >= updateTime && playerSteppedOnPuddle && xFrame < totalFramesX)
    {
        runningTime = 0.0f; // Reset running time

        // Update animation frame
        rectangle.x = rectangle.width * xFrame;
        xFrame++;
    }

    if (playerSteppedOnPuddle)
    {
        steppedOnPuddleTime += Window::Instance()->GetDeltaTime();
        Player::Instance()->SetPlayerNoiseVelocity(50.0f);

        if (steppedOnPuddleTime >= 0.3f)
        {
            xFrame = 0;
            playerSteppedOnPuddle = false;
            steppedOnPuddleTime = 0.0f;
        }
    }

    // If player has collided with the puddle and the bool is false, change the bool to true to trigger puddle animation
    if (Player::Instance()->GetPosition().x + Player::Instance()->GetRectangle().width >= position.x &&
        Player::Instance()->GetPosition().x <= position.x + rectangle.width &&
        Player::Instance()->GetPosition().y + Player::Instance()->GetRectangle().height >= position.y &&
        Player::Instance()->GetPosition().y <= position.y + rectangle.height && !playerSteppedOnPuddle)
    {
        playerSteppedOnPuddle = true;
    }

    // Render puddle when player's light is on
	if (Player::Instance()->GetLightOn()) DrawTextureRec(texture, rectangle, position, color_);
}

void Puddle::UnloadSprite()
{
	UnloadTexture(texture);
}

void Puddle::ResetPuddle(std::string childNode2_)
{
    if (runningTime != 0.0f) runningTime = 0.0f;
    if (xFrame != 0) xFrame = 0;
    if (playerSteppedOnPuddle != false) playerSteppedOnPuddle = false;

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
