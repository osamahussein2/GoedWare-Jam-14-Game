#include "Player.h"
#include "Window.h"
#include "World.h"

std::shared_ptr<Player> Player::playerInstance = nullptr;

const std::string fullTag = "Full";
const std::string currentTag = "Current";

Player::Player() : lightOn(false), footstepsIndexSet(false), footstepsIndex(0), inputEnabled(true), hasFailed(false),
center(), lastPosition()
{
}

Player::~Player()
{
}

std::shared_ptr<Player> Player::Instance()
{
    if (playerInstance == nullptr)
    {
        playerInstance = std::make_shared<Player>();
        return playerInstance;
    }

    return playerInstance;
}

void Player::InitializeCharacter()
{
    document.parse<0>(characterFile.data());

    rootNode = document.first_node("Character");

    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("CharacterInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node("Player"); fileNode2;
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

    camera.target = { position.x, position.y };
    camera.offset = { Window::Instance()->GetWindowWidth() / 2.15f, Window::Instance()->GetWindowHeight() / 2.0f };
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;

    center = { position.x + 40.0f, position.y + 10.0f };

    noiseBars[fullTag].InitializeSprite("FullNoiseBar");
    noiseBars[currentTag].InitializeCurrentBar(currentTag);

    bushes["topBushTile"].InitializeGameObject("Bush1");
    bushes["topLeftBushTile"].InitializeGameObject("Bush2");
    bushes["topRightBushTile"].InitializeGameObject("Bush3");
    bushes["lowerTopLeftBushTile"].InitializeGameObject("Bush4");
    bushes["lowerTopRightBushTile"].InitializeGameObject("Bush5");
    bushes["middleLeftBushTile"].InitializeGameObject("Bush6");
    bushes["middleRightBushTile"].InitializeGameObject("Bush7");
    bushes["topBottomLeftBushTile"].InitializeGameObject("Bush8");
    bushes["topBottomRightBushTile"].InitializeGameObject("Bush9");
    bushes["bottomMiddleBushTile"].InitializeGameObject("Bush10");

    timer.InitializeTimer();

    for (int i = 0; i < FOOTSTEPS_SIZE; i++) groundFootsteps[i].InitializeSound("FootstepsGround" + std::to_string(i + 1));
}

void Player::BeginFollowPlayerCamera()
{
    BeginMode2D(camera);
}

void Player::DrawCharacter()
{
    bushes["topBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
    bushes["topLeftBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
    bushes["topRightBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
    bushes["lowerTopLeftBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
    bushes["lowerTopRightBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
    bushes["middleLeftBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
    bushes["middleRightBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
    bushes["topBottomLeftBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
    bushes["topBottomRightBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
    bushes["bottomMiddleBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });


    // Randomize which footsteps sound to play every frame if it's index isn't set yet
    if (footstepsIndexSet == false && inputEnabled) footstepsIndex = rand() % FOOTSTEPS_SIZE;

    // Increment running time for checking if it reaches update animation time
    if (inputEnabled) runningTime += Window::Instance()->GetDeltaTime();

    if (runningTime >= updateTime && inputEnabled)
    {
        runningTime = 0.0f;

        // Update animation frame
        rectangle.x = rectangle.width * (xFrame % totalFramesX);
        xFrame++;
    }

    if (rectangle.y != rectangle.height * yFrame && inputEnabled) rectangle.y = rectangle.height * yFrame;

    // Make camera target x axis follow the player around if player is inside the x axis world bounds
    if (position.x >= World::Instance()->GetPosition().x && position.x <= World::Instance()->GetRectangle().width)
    {
        camera.target.x = position.x;
    }

    // Make camera target y axis follow the player around if player is inside the y axis world bounds
    if (position.y >= World::Instance()->GetPosition().y && position.y <= World::Instance()->GetRectangle().height)
    {
        camera.target.y = position.y;
    }

    // Call the check for bush collision function for collision detection and response
    CheckForBushCollision();

    Vector2 velocity{};
    if (IsKeyDown(KEY_W) && inputEnabled || IsKeyDown(KEY_UP) && inputEnabled)
    {
        // Play ground footsteps sound
        if (footstepsIndexSet != true) footstepsIndexSet = true;
        groundFootsteps[footstepsIndex].PlaySoundAudio();

        // Play the character moving up animation
        if (yFrame != 1) yFrame = 1;

        // Increase current noise value
        if (noiseBars[currentTag].isNoiseIncreased != true) noiseBars[currentTag].isNoiseIncreased = true;

        // Set velocity to move up only if the player's y position is greater than the world map's y position
        velocity.y = -100.0f;
    }
    if (IsKeyDown(KEY_S) && inputEnabled || IsKeyDown(KEY_DOWN) && inputEnabled)
    {
        // Play ground footsteps sound
        if (footstepsIndexSet != true) footstepsIndexSet = true;
        groundFootsteps[footstepsIndex].PlaySoundAudio();

        // Play the character moving down animation
        if (yFrame != 0) yFrame = 0;

        // Increase current noise value
        if (noiseBars[currentTag].isNoiseIncreased != true) noiseBars[currentTag].isNoiseIncreased = true;

        // Set velocity to move down only if the player's y position is less than the world map's height
        velocity.y = 100.0f;
    }
    if (IsKeyDown(KEY_A) && inputEnabled || IsKeyDown(KEY_LEFT) && inputEnabled)
    {
        // Play ground footsteps sound
        if (footstepsIndexSet != true) footstepsIndexSet = true;
        groundFootsteps[footstepsIndex].PlaySoundAudio();

        // Play the character moving left animation
        if (yFrame != 2) yFrame = 2;

        // Increase current noise value
        if (noiseBars[currentTag].isNoiseIncreased != true) noiseBars[currentTag].isNoiseIncreased = true;

        // Set velocity to move left only if the player's x position is greater than the world map's x position
        velocity.x = -100.0f;
    }
    if (IsKeyDown(KEY_D) && inputEnabled || IsKeyDown(KEY_RIGHT) && inputEnabled)
    {
        // Play ground footsteps sound
        if (footstepsIndexSet != true) footstepsIndexSet = true;
        groundFootsteps[footstepsIndex].PlaySoundAudio();

        // Play the character moving right animation
        if (yFrame != 2) yFrame = 2;

        // Increase current noise value
        if (noiseBars[currentTag].isNoiseIncreased != true) noiseBars[currentTag].isNoiseIncreased = true;

        // Set velocity to move right only if the player's x position is less than the world map's width
        velocity.x = 100.0f;
    }

    else if (!IsKeyDown(KEY_W) && !IsKeyDown(KEY_UP) && !IsKeyDown(KEY_S) && !IsKeyDown(KEY_DOWN) &&
        !IsKeyDown(KEY_A) && !IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_RIGHT) && inputEnabled)
    {
        // Don't play ground footsteps sound anymore and decrease current noise value if isn't already
        if (footstepsIndexSet != false) footstepsIndexSet = false;
        if (noiseBars[currentTag].isNoiseIncreased != false) noiseBars[currentTag].isNoiseIncreased = false;

        // Play the character idle animation
        if (yFrame != 3) yFrame = 3;
    }

    if (IsKeyPressed(KEY_F) && inputEnabled) lightOn = !lightOn;

    // If light isn't on, draw a black rectangle to give the illusion that lights are off
    if (!lightOn)
    {
        // Don't increase noise value when player isn't moving via movement input
        if (!IsKeyDown(KEY_W) && !IsKeyDown(KEY_UP) && !IsKeyDown(KEY_S) && !IsKeyDown(KEY_DOWN) &&
            !IsKeyDown(KEY_A) && !IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_RIGHT) && inputEnabled)
        {
            if (noiseBars[currentTag].isNoiseIncreased != false) noiseBars[currentTag].isNoiseIncreased = false;
        }
    }

    // But if light is on, draw a slightly transparent black rectangle to give the illusion that lights are on
    else if (lightOn)
    {
        // Draw circle as a light for the player
        DrawCircleGradient(center.x, center.y, circleRadius, Color{ 255, 255, 255, 150 }, Color{ 255, 255, 255, 30 });

        if (noiseBars[currentTag].isNoiseIncreased != true) noiseBars[currentTag].isNoiseIncreased = true;
    }

    // Animate the player's facing direction according to last recorded velocity before stopping
    if (velocity.x < 0.0f) rectangle.width = -1.0f * static_cast<float>(texture.width) / totalFramesX;
    else if (velocity.x > 0.0f) rectangle.width = 1.0f * static_cast<float>(texture.width) / totalFramesX;

    // Move the player's position by using velocity and delta time
    position = Vector2Add(position, Vector2Scale(velocity, Window::Instance()->GetDeltaTime()));
    center = Vector2Add(center, Vector2Scale(velocity, Window::Instance()->GetDeltaTime()));

    // Render the player
    if (lightOn) DrawTextureRec(texture, rectangle, position, WHITE);

    EndMode2D();
}

void Player::UnloadCharacter()
{
    // Unload the player texture
    UnloadTexture(texture);

    bushes["topBushTile"].UnloadSprite();
    bushes["topLeftBushTile"].UnloadSprite();
    bushes["topRightBushTile"].UnloadSprite();
    bushes["lowerTopLeftBushTile"].UnloadSprite();
    bushes["lowerTopRightBushTile"].UnloadSprite();
    bushes["middleLeftBushTile"].UnloadSprite();
    bushes["middleRightBushTile"].UnloadSprite();
    bushes["topBottomLeftBushTile"].UnloadSprite();
    bushes["topBottomRightBushTile"].UnloadSprite();
    bushes["bottomMiddleBushTile"].UnloadSprite();
}

void Player::DrawUI()
{
    noiseBars[fullTag].UpdateCurrentNoise(fullTag);
    noiseBars[currentTag].UpdateCurrentNoise(currentTag);

    noiseBars[fullTag].DrawSprite(Color{ 125, 125, 125, 255 });
    noiseBars[currentTag].DrawSprite(RED);

    timer.RenderTimer(WHITE);
}

// Getter functions for the Player class
int Player::GetNoiseValue()
{
    return noiseBars[currentTag].GetCurrentNoise();
}

int Player::GetMaxNoiseThresholdValue()
{
    return noiseBars[currentTag].GetMaxNoiseThreshold();
}

bool Player::GetCurrentNoiseMaxedOut()
{
    return noiseBars[currentTag].GetNoiseMaxedOut();
}

void Player::SetPlayerInputEnabled(bool inputEnabled_)
{
    if (inputEnabled != inputEnabled_) inputEnabled = inputEnabled_;
}

void Player::ReturnMaximumNoise()
{
    if (!noiseBars[currentTag].GetNoiseMaxedOut()) noiseBars[currentTag].SetCurrentNoiseToMaximum();
}

bool Player::HasFailed() const
{
    return hasFailed;
}

void Player::SetHasFailed(bool hasFailed_)
{
    if (hasFailed != hasFailed_) hasFailed = hasFailed_;
}

void Player::ResetCharacter()
{
    inputEnabled = true;
    hasFailed = false;

    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("CharacterInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node("Player"); fileNode2;
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

    camera.target = { position.x, position.y };
    camera.offset = { Window::Instance()->GetWindowWidth() / 2.15f, Window::Instance()->GetWindowHeight() / 2.0f };
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;

    center = { position.x + 40.0f, position.y + 10.0f };

    lightOn = false;

    xFrame = 0;
    yFrame = 0;

    noiseBars[currentTag].ResetCurrentNoiseValue();
    timer.ResetTimer();
}

void Player::CheckForBushCollision()
{
    // Check if player collided with any of the bushes
    if (position.x + rectangle.width >= bushes["topBushTile"].GetPosition().x &&
        position.x <= bushes["topBushTile"].GetPosition().x + bushes["topBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= bushes["topBushTile"].GetPosition().y &&
        position.y <= bushes["topBushTile"].GetPosition().y + bushes["topBushTile"].GetRectangle().height
        || 
        position.x + rectangle.width >= bushes["topLeftBushTile"].GetPosition().x &&
        position.x <= bushes["topLeftBushTile"].GetPosition().x + bushes["topLeftBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= bushes["topLeftBushTile"].GetPosition().y &&
        position.y <= bushes["topLeftBushTile"].GetPosition().y + bushes["topLeftBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= bushes["topRightBushTile"].GetPosition().x &&
        position.x <= bushes["topRightBushTile"].GetPosition().x + bushes["topRightBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= bushes["topRightBushTile"].GetPosition().y &&
        position.y <= bushes["topRightBushTile"].GetPosition().y + bushes["topRightBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= bushes["lowerTopLeftBushTile"].GetPosition().x &&
        position.x <= bushes["lowerTopLeftBushTile"].GetPosition().x + bushes["lowerTopLeftBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= bushes["lowerTopLeftBushTile"].GetPosition().y &&
        position.y <= bushes["lowerTopLeftBushTile"].GetPosition().y + bushes["lowerTopLeftBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= bushes["lowerTopRightBushTile"].GetPosition().x &&
        position.x <= bushes["lowerTopRightBushTile"].GetPosition().x + bushes["lowerTopRightBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= bushes["lowerTopRightBushTile"].GetPosition().y &&
        position.y <= bushes["lowerTopRightBushTile"].GetPosition().y + bushes["lowerTopRightBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= bushes["middleLeftBushTile"].GetPosition().x &&
        position.x <= bushes["middleLeftBushTile"].GetPosition().x + bushes["middleLeftBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= bushes["middleLeftBushTile"].GetPosition().y &&
        position.y <= bushes["middleLeftBushTile"].GetPosition().y + bushes["middleLeftBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= bushes["middleRightBushTile"].GetPosition().x &&
        position.x <= bushes["middleRightBushTile"].GetPosition().x + bushes["middleRightBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= bushes["middleRightBushTile"].GetPosition().y &&
        position.y <= bushes["middleRightBushTile"].GetPosition().y + bushes["middleRightBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= bushes["topBottomLeftBushTile"].GetPosition().x &&
        position.x <= bushes["topBottomLeftBushTile"].GetPosition().x + bushes["topBottomLeftBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= bushes["topBottomLeftBushTile"].GetPosition().y &&
        position.y <= bushes["topBottomLeftBushTile"].GetPosition().y + bushes["topBottomLeftBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= bushes["topBottomRightBushTile"].GetPosition().x &&
        position.x <= bushes["topBottomRightBushTile"].GetPosition().x + bushes["topBottomRightBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= bushes["topBottomRightBushTile"].GetPosition().y &&
        position.y <= bushes["topBottomRightBushTile"].GetPosition().y + bushes["topBottomRightBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= bushes["bottomMiddleBushTile"].GetPosition().x &&
        position.x <= bushes["bottomMiddleBushTile"].GetPosition().x + bushes["bottomMiddleBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= bushes["bottomMiddleBushTile"].GetPosition().y &&
        position.y <= bushes["bottomMiddleBushTile"].GetPosition().y + bushes["bottomMiddleBushTile"].GetRectangle().height)
    {
        if (position != lastPosition) position = lastPosition;
        if (center != lastPosition) center = { lastPosition.x + 40.0f, lastPosition.y + 10.0f };
    }

    else
    {
        if (lastPosition != position) lastPosition = position;
    }
}