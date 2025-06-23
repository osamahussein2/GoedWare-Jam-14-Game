#include "Spectre.h"
#include "Window.h"
#include "Player.h"

Spectre::Spectre() : failedTimer(0.0f)
{
}

Spectre::~Spectre()
{
}

void Spectre::InitializeCharacter(std::string childNode2_)
{
    document.parse<0>(characterFile.data());

    rootNode = document.first_node("Character");

    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("CharacterInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node(childNode2_.c_str()); fileNode2;
            fileNode2 = fileNode->next_sibling())
        {
            totalFramesX = atoi(fileNode2->first_attribute("totalFramesX")->value());
            totalFramesY = atoi(fileNode2->first_attribute("totalFramesY")->value());

            texture = LoadTexture(fileNode2->first_attribute("spritePath")->value());
            texture.width *= atof(fileNode2->first_attribute("scaleMultiplier")->value());
            texture.height *= atof(fileNode2->first_attribute("scaleMultiplier")->value());

            rectangle.width = static_cast<float>(texture.width / totalFramesX);
            rectangle.height = static_cast<float>(texture.height / totalFramesY);
            rectangle.x = 0.0f;
            rectangle.y = 0.0f;

            position = Vector2{ static_cast<float>(atof(fileNode2->first_attribute("posX")->value())),
                static_cast<float>(atof(fileNode2->first_attribute("posY")->value())) };
        }
    }
}

void Spectre::DrawCharacter()
{
    // Increment running time for checking if it reaches update animation time
    runningTime += Window::Instance()->GetDeltaTime();

    if (runningTime >= updateTime && Player::Instance()->GetPlayerInputEnabled())
    {
        runningTime = 0.0f;

        // Update animation frame
        rectangle.x = rectangle.width * (xFrame % totalFramesX);
        xFrame++;
    }

    // Set the rectangle's y for change in animation state
    if (rectangle.y != rectangle.height * yFrame) rectangle.y = rectangle.height * yFrame;

    // Make sure the player's noise value reaches the max noise threshold
    if (Player::Instance()->GetCurrentNoiseMaxedOut())
    {
        // Use distance forumla
        float dx = Player::Instance()->GetPosition().x - position.x;
        float dy = Player::Instance()->GetPosition().y - position.y;
        float distance = sqrtf((dx * dx) + (dy * dy));

        if (distance >= distanceThreshold)
        {
            // Get the direction vector
            Vector2 goToPlayer = Vector2Subtract(Player::Instance()->GetPosition(), position);

            // Normalize direction vector
            goToPlayer = Vector2Normalize(goToPlayer);

            // Multiply direction vector by speed
            goToPlayer = Vector2Scale(goToPlayer, 4.0f);

            // Move spectre towards player
            position = Vector2Add(position, goToPlayer);
        }

        // If the spectre gets close enough to the player, trigger fail state logic
        else if (distance < distanceThreshold)
        {
            Player::Instance()->SetPlayerInputEnabled(false); // Disable any player input

            failedTimer += Window::Instance()->GetDeltaTime(); // Increment game over time

            // Set failed to true to transition to the fail game state
            if (failedTimer >= 2.0f && !Player::Instance()->HasFailed())
            {
                Player::Instance()->SetHasFailed(true);
                failedTimer = 0.0f;
            }
        }
    }

    else if (!Player::Instance()->GetCurrentNoiseMaxedOut())
    {
        // Play the character idle animation
        if (yFrame != 3) yFrame = 3;
    }

    // Render the spectre
    if (Player::Instance()->GetLightOn()) DrawTextureRec(texture, rectangle, position, WHITE);
}

void Spectre::UnloadCharacter()
{
    UnloadTexture(texture);
}

void Spectre::ResetCharacter(std::string childNode2_)
{
    if (failedTimer != 0.0f) failedTimer = 0.0f;

    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("CharacterInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node(childNode2_.c_str()); fileNode2;
            fileNode2 = fileNode->next_sibling())
        {
            totalFramesX = atoi(fileNode2->first_attribute("totalFramesX")->value());
            totalFramesY = atoi(fileNode2->first_attribute("totalFramesY")->value());

            texture = LoadTexture(fileNode2->first_attribute("spritePath")->value());
            texture.width *= atof(fileNode2->first_attribute("scaleMultiplier")->value());
            texture.height *= atof(fileNode2->first_attribute("scaleMultiplier")->value());

            rectangle.width = static_cast<float>(texture.width / totalFramesX);
            rectangle.height = static_cast<float>(texture.height / totalFramesY);
            rectangle.x = 0.0f;
            rectangle.y = 0.0f;

            position = Vector2{ static_cast<float>(atof(fileNode2->first_attribute("posX")->value())),
                static_cast<float>(atof(fileNode2->first_attribute("posY")->value())) };
        }
    }
}