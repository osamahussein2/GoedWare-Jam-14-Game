#include "Spectre.h"
#include "Window.h"
#include "Player.h"

Spectre::Spectre() : failedTimer(0.0f), spectreState(SpectreState::SLEEPING), updateTime()
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
    ChangeUpdateAnimationTime(); // Call the update animation time function

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

    /* If the spectre inside the player's collision bounds, set current noise to maximum if it isn't already to act as if
    the player is way too close to them */
    if (position.x + rectangle.width >= Player::Instance()->GetPosition().x &&
        position.x <= Player::Instance()->GetPosition().x + Player::Instance()->GetRectangle().width &&
        position.y + rectangle.height >= Player::Instance()->GetPosition().y &&
        position.y <= Player::Instance()->GetPosition().y + Player::Instance()->GetRectangle().height)
    {
        Player::Instance()->ReturnMaximumNoise();
    }

    // Make sure the player's noise value reaches the max noise threshold
    if (Player::Instance()->GetCurrentNoiseMaxedOut())
    {
        // Use distance forumla
        float dx = Player::Instance()->GetPosition().x - position.x;
        float dy = Player::Instance()->GetPosition().y - position.y;
        float distance = sqrtf((dx * dx) + (dy * dy));

        // If spectre is sleeping and not awake, transition to the wake up state
        if (spectreState == SpectreState::SLEEPING && spectreState != SpectreState::WAKING_UP)
        {
            yFrame = 1;
            runningTime = 0.0f;
            xFrame = 0;

            spectreState = SpectreState::WAKING_UP;
        }

        /* Else if spectre is waking up and the current x frame exceeds the total x frame number of the sprite, transition
        to moving state and reset animation frame values to 0 */
        else if (spectreState == SpectreState::WAKING_UP && xFrame >= totalFramesX)
        {
            runningTime = 0.0f;
            xFrame = 0;
            spectreState = SpectreState::MOVING;
        }

        /* Once the spectre state is equal to moving and its distance to the player is greater than the distance threshold,
        start chasing the player */
        if (distance >= distanceThreshold && spectreState == SpectreState::MOVING)
        {
            // Get the direction vector
            Vector2 goToPlayer = Vector2Subtract(Player::Instance()->GetPosition(), position);

            // Normalize direction vector
            goToPlayer = Vector2Normalize(goToPlayer);

            // Multiply direction vector by speed
            goToPlayer = Vector2Scale(goToPlayer, 4.0f);

            // Move spectre towards player
            position = Vector2Add(position, goToPlayer);

            if (goToPlayer.y <= -0.1f)
            {
                // Play the character down animation
                if (yFrame != 2) yFrame = 2;
            }

            else if (goToPlayer.y >= 0.1f)
            {
                // Play the character up animation
                if (yFrame != 3) yFrame = 3;
            }

            else if (goToPlayer.x <= -0.1f)
            {
                // Flip rectangle horizontally for sprite x flip
                if (rectangle.width != -1.0f * static_cast<float>(texture.width) / totalFramesX)
                    rectangle.width = -1.0f * static_cast<float>(texture.width) / totalFramesX;

                // Play the character left animation
                if (yFrame != 4) yFrame = 4;
            }

            else if (goToPlayer.x >= 0.1f)
            {
                // Don't flip rectangle horizontally
                if (rectangle.width != 1.0f * static_cast<float>(texture.width) / totalFramesX)
                    rectangle.width = 1.0f * static_cast<float>(texture.width) / totalFramesX;

                // Play the character right animation
                if (yFrame != 4) yFrame = 4;
            }
        }

        // If the spectre gets close enough to the player and its state is set to moving, trigger fail state logic
        else if (distance < distanceThreshold && spectreState == SpectreState::MOVING)
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

    // Otherwise if the player hasn't caused maximum amount of noise yet
    else if (!Player::Instance()->GetCurrentNoiseMaxedOut())
    {
        // Play the character sleeping animation
        if (yFrame != 0) yFrame = 0;
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

    if (xFrame != 0) xFrame = 0;
    if (yFrame != 0) yFrame = 0;

    if (spectreState != SpectreState::SLEEPING) spectreState = SpectreState::SLEEPING;

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

void Spectre::ChangeUpdateAnimationTime()
{
    // Change update time animation depending on current y animation frame
    switch (yFrame)
    {
    case 0:
        if (updateTime != float{ 1.0f / 6.0f }) updateTime = { 1.0f / 6.0f };
        break;
    case 1:
        if (updateTime != float{ 1.0f / 3.0f }) updateTime = { 1.0f / 3.0f };
        break;
    case 2:
        if (updateTime != float{ 1.0f / 12.0f }) updateTime = { 1.0f / 12.0f };
        break;
    case 3:
        if (updateTime != float{ 1.0f / 12.0f }) updateTime = { 1.0f / 12.0f };
        break;
    case 4:
        if (updateTime != float{ 1.0f / 12.0f }) updateTime = { 1.0f / 12.0f };
        break;
    default:
        break;
    }
}