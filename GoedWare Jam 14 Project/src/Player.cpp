#include "Player.h"
#include "Window.h"
#include "World.h"

std::shared_ptr<Player> Player::playerInstance = nullptr;

constexpr float noiseVelocity = 25.0f;

const std::string fullTag = "Full";
const std::string currentTag = "Current";

Player::Player() : lightOn(false), footstepsIndexSet(false), footstepsIndex(0), inputEnabled(true), hasFailed(false),
center(), lastPosition(), levelNumber(1)
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

    level1Bushes["topBushTile"].InitializeGameObject("Bush1");
    level1Bushes["topLeftBushTile"].InitializeGameObject("Bush2");
    level1Bushes["topRightBushTile"].InitializeGameObject("Bush3");
    level1Bushes["lowerTopLeftBushTile"].InitializeGameObject("Bush4");
    level1Bushes["lowerTopRightBushTile"].InitializeGameObject("Bush5");
    level1Bushes["middleLeftBushTile"].InitializeGameObject("Bush6");
    level1Bushes["middleRightBushTile"].InitializeGameObject("Bush7");
    level1Bushes["topBottomLeftBushTile"].InitializeGameObject("Bush8");
    level1Bushes["topBottomRightBushTile"].InitializeGameObject("Bush9");
    level1Bushes["bottomMiddleBushTile"].InitializeGameObject("Bush10");
    level1Bushes["topLeftBlockTile"].InitializeGameObject("Bush11");
    level1Bushes["topRightBlockTile"].InitializeGameObject("Bush12");
    level1Bushes["lowerTopLeftBlockTile"].InitializeGameObject("Bush13");
    level1Bushes["lowerTopRightBlockTile"].InitializeGameObject("Bush14");
    level1Bushes["topBottomLeftBlockTile"].InitializeGameObject("Bush15");
    level1Bushes["topBottomRightBlockTile"].InitializeGameObject("Bush16");

    stair.InitializeGameObject("Stair");

    level2Bushes["leftBushTile"].InitializeGameObject("Bush2-1");
    level2Bushes["rightBushTile"].InitializeGameObject("Bush2-2");
    level2Bushes["topLeftBushTile"].InitializeGameObject("Bush2-3");
    level2Bushes["topRightBushTile"].InitializeGameObject("Bush2-4");
    level2Bushes["bottomBushTile"].InitializeGameObject("Bush2-5");

    timer.InitializeTimer();

    for (int i = 0; i < FOOTSTEPS_SIZE; i++) groundFootsteps[i].InitializeSound("FootstepsGround" + std::to_string(i + 1));

    for (int i = 0; i < level1Puddles.size(); i++) level1Puddles[i].InitializeGameObject("Puddle" + std::to_string(i + 1));
    for (int i = 0; i < level1Branches.size(); i++) level1Branches[i].InitializeGameObject("Branch" + std::to_string(i + 1));

    for (int i = 0; i < level2Puddles.size(); i++) level2Puddles[i].InitializeGameObject("Puddle2." + std::to_string(i + 1));
    for (int i = 0; i < level2Branches.size(); i++) level2Branches[i].InitializeGameObject("Branch2." + std::to_string(i + 1));
}

void Player::BeginFollowPlayerCamera()
{
    BeginMode2D(camera);
}

void Player::DrawCharacter()
{
    // Draw and update certain logic depending on current level number
    switch (levelNumber)
    {
    case 1:
        // If player goes out of map bounds, respawn the player back inside the map bounds
        if (position.y < 0.0f || position.y > level1Bushes["bottomMiddleBushTile"].GetPosition().y +
            level1Bushes["bottomMiddleBushTile"].GetRectangle().height || position.x < 0.0f || 
            position.x > level1Bushes["middleRightBushTile"].GetPosition().x +
            level1Bushes["middleRightBushTile"].GetRectangle().width)
        {
            position = { 830.0f, 255.0f };
            center = { position.x + 40.0f, position.y + 10.0f };
        }

        // Call the check for bush collision function for collision detection and response
        CheckForLevel1BushCollision();

        break;

    case 2:
        // If player goes out of map bounds, respawn the player back inside the map bounds
        if (position.y < -100.0f || position.y > level2Bushes["bottomBushTile"].GetPosition().y +
            level2Bushes["bottomBushTile"].GetRectangle().height || position.x < 0.0f || 
            position.x > level2Bushes["rightBushTile"].GetPosition().x + level2Bushes["rightBushTile"].GetRectangle().width)
        {
            position = { 334.0f, 1390.0f };
            center = { position.x + 40.0f, position.y + 10.0f };
        }

        // Call the check for bush collision function for collision detection and response
        CheckForLevel2BushCollision();
        break;

    default:
        break;
    }

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
    camera.target = position;

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

        switch (levelNumber)
        {
        case 1:
            if (!level1Puddles[0].GetPlayerSteppedOnPuddle() && !level1Puddles[1].GetPlayerSteppedOnPuddle() &&
                !level1Puddles[2].GetPlayerSteppedOnPuddle() && !level1Puddles[3].GetPlayerSteppedOnPuddle() &&
                !level1Branches[0].GetPlayerSteppedOnBranch() && !level1Branches[1].GetPlayerSteppedOnBranch() &&
                !level1Branches[2].GetPlayerSteppedOnBranch() && !level1Branches[3].GetPlayerSteppedOnBranch())
                noiseBars[currentTag].SetCurrentNoiseVelocity(noiseVelocity);
            break;

        case 2:
            if (!level2Puddles[0].GetPlayerSteppedOnPuddle() && !level2Puddles[1].GetPlayerSteppedOnPuddle() &&
                !level2Puddles[2].GetPlayerSteppedOnPuddle() && !level2Puddles[3].GetPlayerSteppedOnPuddle() &&
                !level2Puddles[4].GetPlayerSteppedOnPuddle() && !level2Branches[0].GetPlayerSteppedOnBranch() && 
                !level2Branches[1].GetPlayerSteppedOnBranch() && !level2Branches[2].GetPlayerSteppedOnBranch())
                noiseBars[currentTag].SetCurrentNoiseVelocity(noiseVelocity);
            break;

        default:
            break;
        }

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

        switch (levelNumber)
        {
        case 1:
            if (!level1Puddles[0].GetPlayerSteppedOnPuddle() && !level1Puddles[1].GetPlayerSteppedOnPuddle() &&
                !level1Puddles[2].GetPlayerSteppedOnPuddle() && !level1Puddles[3].GetPlayerSteppedOnPuddle() &&
                !level1Branches[0].GetPlayerSteppedOnBranch() && !level1Branches[1].GetPlayerSteppedOnBranch() &&
                !level1Branches[2].GetPlayerSteppedOnBranch() && !level1Branches[3].GetPlayerSteppedOnBranch())
                noiseBars[currentTag].SetCurrentNoiseVelocity(noiseVelocity);
            break;

        case 2:
            if (!level2Puddles[0].GetPlayerSteppedOnPuddle() && !level2Puddles[1].GetPlayerSteppedOnPuddle() &&
                !level2Puddles[2].GetPlayerSteppedOnPuddle() && !level2Puddles[3].GetPlayerSteppedOnPuddle() &&
                !level2Puddles[4].GetPlayerSteppedOnPuddle() && !level2Branches[0].GetPlayerSteppedOnBranch() &&
                !level2Branches[1].GetPlayerSteppedOnBranch() && !level2Branches[2].GetPlayerSteppedOnBranch())
                noiseBars[currentTag].SetCurrentNoiseVelocity(noiseVelocity);
            break;

        default:
            break;
        }

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

        switch (levelNumber)
        {
        case 1:
            if (!level1Puddles[0].GetPlayerSteppedOnPuddle() && !level1Puddles[1].GetPlayerSteppedOnPuddle() &&
                !level1Puddles[2].GetPlayerSteppedOnPuddle() && !level1Puddles[3].GetPlayerSteppedOnPuddle() &&
                !level1Branches[0].GetPlayerSteppedOnBranch() && !level1Branches[1].GetPlayerSteppedOnBranch() &&
                !level1Branches[2].GetPlayerSteppedOnBranch() && !level1Branches[3].GetPlayerSteppedOnBranch())
                noiseBars[currentTag].SetCurrentNoiseVelocity(noiseVelocity);
            break;

        case 2:
            if (!level2Puddles[0].GetPlayerSteppedOnPuddle() && !level2Puddles[1].GetPlayerSteppedOnPuddle() &&
                !level2Puddles[2].GetPlayerSteppedOnPuddle() && !level2Puddles[3].GetPlayerSteppedOnPuddle() &&
                !level2Puddles[4].GetPlayerSteppedOnPuddle() && !level2Branches[0].GetPlayerSteppedOnBranch() &&
                !level2Branches[1].GetPlayerSteppedOnBranch() && !level2Branches[2].GetPlayerSteppedOnBranch())
                noiseBars[currentTag].SetCurrentNoiseVelocity(noiseVelocity);
            break;

        default:
            break;
        }

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

        switch (levelNumber)
        {
        case 1:
            if (!level1Puddles[0].GetPlayerSteppedOnPuddle() && !level1Puddles[1].GetPlayerSteppedOnPuddle() &&
                !level1Puddles[2].GetPlayerSteppedOnPuddle() && !level1Puddles[3].GetPlayerSteppedOnPuddle() &&
                !level1Branches[0].GetPlayerSteppedOnBranch() && !level1Branches[1].GetPlayerSteppedOnBranch() &&
                !level1Branches[2].GetPlayerSteppedOnBranch() && !level1Branches[3].GetPlayerSteppedOnBranch())
                noiseBars[currentTag].SetCurrentNoiseVelocity(noiseVelocity);
            break;

        case 2:
            if (!level2Puddles[0].GetPlayerSteppedOnPuddle() && !level2Puddles[1].GetPlayerSteppedOnPuddle() &&
                !level2Puddles[2].GetPlayerSteppedOnPuddle() && !level2Puddles[3].GetPlayerSteppedOnPuddle() &&
                !level2Puddles[4].GetPlayerSteppedOnPuddle() && !level2Branches[0].GetPlayerSteppedOnBranch() &&
                !level2Branches[1].GetPlayerSteppedOnBranch() && !level2Branches[2].GetPlayerSteppedOnBranch())
                noiseBars[currentTag].SetCurrentNoiseVelocity(noiseVelocity);
            break;

        default:
            break;
        }

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

    // If player collided with stairs, go to the next area
    if (position.x + rectangle.width >= stair.GetPosition().x &&
        position.x <= stair.GetPosition().x + stair.GetRectangle().width &&
        position.y + rectangle.height >= stair.GetPosition().y &&
        position.y <= stair.GetPosition().y + stair.GetRectangle().height && levelNumber == 1)
    {
        position = { 334.0f, 1390.0f };
        center = { position.x + 40.0f, position.y + 10.0f };

        if (!noiseBars[currentTag].GetNoiseMaxedOut()) noiseBars[currentTag].ResetCurrentNoiseValue();

        levelNumber = 2;
    }

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

        switch (levelNumber)
        {
        case 1:
            if (!level1Puddles[0].GetPlayerSteppedOnPuddle() && !level1Puddles[1].GetPlayerSteppedOnPuddle() &&
                !level1Puddles[2].GetPlayerSteppedOnPuddle() && !level1Puddles[3].GetPlayerSteppedOnPuddle() &&
                !level1Branches[0].GetPlayerSteppedOnBranch() && !level1Branches[1].GetPlayerSteppedOnBranch() &&
                !level1Branches[2].GetPlayerSteppedOnBranch() && !level1Branches[3].GetPlayerSteppedOnBranch())
                noiseBars[currentTag].SetCurrentNoiseVelocity(noiseVelocity);
            break;

        case 2:
            if (!level2Puddles[0].GetPlayerSteppedOnPuddle() && !level2Puddles[1].GetPlayerSteppedOnPuddle() &&
                !level2Puddles[2].GetPlayerSteppedOnPuddle() && !level2Puddles[3].GetPlayerSteppedOnPuddle() &&
                !level2Puddles[4].GetPlayerSteppedOnPuddle() && !level2Branches[0].GetPlayerSteppedOnBranch() &&
                !level2Branches[1].GetPlayerSteppedOnBranch() && !level2Branches[2].GetPlayerSteppedOnBranch())
                noiseBars[currentTag].SetCurrentNoiseVelocity(noiseVelocity);
            break;

        default:
            break;
        }

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

    for (std::pair<const std::string, GameObject>& bushMaps : level1Bushes)
    {
        bushMaps.second.UnloadSprite();
    }

    for (std::pair<const std::string, GameObject>& bushMaps2 : level2Bushes)
    {
        bushMaps2.second.UnloadSprite();
    }

    for (int i = 0; i < level1Puddles.size(); i++) level1Puddles[i].UnloadSprite();
    for (int i = 0; i < level1Branches.size(); i++) level1Branches[i].UnloadSprite();

    for (int i = 0; i < level2Puddles.size(); i++) level2Puddles[i].UnloadSprite();
    for (int i = 0; i < level2Branches.size(); i++) level2Branches[i].UnloadSprite();

    stair.UnloadSprite();
}

void Player::DrawGameObjects()
{
    switch (levelNumber)
    {
    case 1:
        level1Bushes["topBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level1Bushes["topLeftBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level1Bushes["topRightBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level1Bushes["lowerTopLeftBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level1Bushes["lowerTopRightBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level1Bushes["middleLeftBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level1Bushes["middleRightBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level1Bushes["topBottomLeftBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level1Bushes["topBottomRightBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level1Bushes["bottomMiddleBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level1Bushes["topLeftBlockTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level1Bushes["topRightBlockTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level1Bushes["lowerTopLeftBlockTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level1Bushes["lowerTopRightBlockTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level1Bushes["topBottomLeftBlockTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level1Bushes["topBottomRightBlockTile"].DrawSprite(Color{ 255, 255, 255, 150 });

        for (int i = 0; i < level1Puddles.size(); i++) level1Puddles[i].DrawSprite(Color{255, 255, 255, 150});
        for (int i = 0; i < level1Branches.size(); i++) level1Branches[i].DrawSprite(Color{255, 255, 255, 150});

        stair.DrawSprite(Color{ 255, 255, 255, 150 });

        break;

    case 2:
        level2Bushes["leftBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level2Bushes["rightBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level2Bushes["topLeftBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level2Bushes["topRightBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });
        level2Bushes["bottomBushTile"].DrawSprite(Color{ 255, 255, 255, 150 });

        for (int i = 0; i < level2Puddles.size(); i++) level2Puddles[i].DrawSprite(Color{ 255, 255, 255, 150 });
        for (int i = 0; i < level2Branches.size(); i++) level2Branches[i].DrawSprite(Color{ 255, 255, 255, 150 });

        break;

    default:
        break;
    }
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

void Player::SetPlayerNoiseVelocity(float noiseVelocity_)
{
    noiseBars[currentTag].SetCurrentNoiseVelocity(noiseVelocity_);
}

void Player::SetHasFailed(bool hasFailed_)
{
    if (hasFailed != hasFailed_) hasFailed = hasFailed_;
}

void Player::ResetCharacter()
{
    inputEnabled = true;
    hasFailed = false;

    if (levelNumber != 1) levelNumber = 1;

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

    for (int i = 0; i < level1Puddles.size(); i++) level1Puddles[i].ResetPuddle("Puddle" + std::to_string(i + 1));
    for (int i = 0; i < level1Branches.size(); i++) level1Branches[i].ResetBranch("Branch" + std::to_string(i + 1));

    for (int i = 0; i < level2Puddles.size(); i++) level2Puddles[i].ResetPuddle("Puddle2." + std::to_string(i + 1));
    for (int i = 0; i < level2Branches.size(); i++) level2Branches[i].ResetBranch("Branch2." + std::to_string(i + 1));
}

void Player::CheckForLevel1BushCollision()
{
    // Check if player collided with any of the level1Bushes
    if (position.x + rectangle.width >= level1Bushes["topBushTile"].GetPosition().x &&
        position.x <= level1Bushes["topBushTile"].GetPosition().x + level1Bushes["topBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= level1Bushes["topBushTile"].GetPosition().y &&
        position.y <= level1Bushes["topBushTile"].GetPosition().y + level1Bushes["topBushTile"].GetRectangle().height
        || 
        position.x + rectangle.width >= level1Bushes["topLeftBushTile"].GetPosition().x &&
        position.x <= level1Bushes["topLeftBushTile"].GetPosition().x + level1Bushes["topLeftBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= level1Bushes["topLeftBushTile"].GetPosition().y &&
        position.y <= level1Bushes["topLeftBushTile"].GetPosition().y + level1Bushes["topLeftBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level1Bushes["topRightBushTile"].GetPosition().x &&
        position.x <= level1Bushes["topRightBushTile"].GetPosition().x + level1Bushes["topRightBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= level1Bushes["topRightBushTile"].GetPosition().y &&
        position.y <= level1Bushes["topRightBushTile"].GetPosition().y + level1Bushes["topRightBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level1Bushes["lowerTopLeftBushTile"].GetPosition().x &&
        position.x <= level1Bushes["lowerTopLeftBushTile"].GetPosition().x + level1Bushes["lowerTopLeftBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= level1Bushes["lowerTopLeftBushTile"].GetPosition().y &&
        position.y <= level1Bushes["lowerTopLeftBushTile"].GetPosition().y + level1Bushes["lowerTopLeftBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level1Bushes["lowerTopRightBushTile"].GetPosition().x &&
        position.x <= level1Bushes["lowerTopRightBushTile"].GetPosition().x + level1Bushes["lowerTopRightBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= level1Bushes["lowerTopRightBushTile"].GetPosition().y &&
        position.y <= level1Bushes["lowerTopRightBushTile"].GetPosition().y + level1Bushes["lowerTopRightBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level1Bushes["middleLeftBushTile"].GetPosition().x &&
        position.x <= level1Bushes["middleLeftBushTile"].GetPosition().x + level1Bushes["middleLeftBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= level1Bushes["middleLeftBushTile"].GetPosition().y &&
        position.y <= level1Bushes["middleLeftBushTile"].GetPosition().y + level1Bushes["middleLeftBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level1Bushes["middleRightBushTile"].GetPosition().x &&
        position.x <= level1Bushes["middleRightBushTile"].GetPosition().x + level1Bushes["middleRightBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= level1Bushes["middleRightBushTile"].GetPosition().y &&
        position.y <= level1Bushes["middleRightBushTile"].GetPosition().y + level1Bushes["middleRightBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level1Bushes["topBottomLeftBushTile"].GetPosition().x &&
        position.x <= level1Bushes["topBottomLeftBushTile"].GetPosition().x + level1Bushes["topBottomLeftBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= level1Bushes["topBottomLeftBushTile"].GetPosition().y &&
        position.y <= level1Bushes["topBottomLeftBushTile"].GetPosition().y + level1Bushes["topBottomLeftBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level1Bushes["topBottomRightBushTile"].GetPosition().x &&
        position.x <= level1Bushes["topBottomRightBushTile"].GetPosition().x + level1Bushes["topBottomRightBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= level1Bushes["topBottomRightBushTile"].GetPosition().y &&
        position.y <= level1Bushes["topBottomRightBushTile"].GetPosition().y + level1Bushes["topBottomRightBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level1Bushes["bottomMiddleBushTile"].GetPosition().x &&
        position.x <= level1Bushes["bottomMiddleBushTile"].GetPosition().x + level1Bushes["bottomMiddleBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= level1Bushes["bottomMiddleBushTile"].GetPosition().y &&
        position.y <= level1Bushes["bottomMiddleBushTile"].GetPosition().y + level1Bushes["bottomMiddleBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level1Bushes["topLeftBlockTile"].GetPosition().x &&
        position.x <= level1Bushes["topLeftBlockTile"].GetPosition().x + level1Bushes["topLeftBlockTile"].GetRectangle().width &&
        position.y + rectangle.height >= level1Bushes["topLeftBlockTile"].GetPosition().y &&
        position.y <= level1Bushes["topLeftBlockTile"].GetPosition().y + level1Bushes["topLeftBlockTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level1Bushes["topRightBlockTile"].GetPosition().x &&
        position.x <= level1Bushes["topRightBlockTile"].GetPosition().x + level1Bushes["topRightBlockTile"].GetRectangle().width &&
        position.y + rectangle.height >= level1Bushes["topRightBlockTile"].GetPosition().y &&
        position.y <= level1Bushes["topRightBlockTile"].GetPosition().y + level1Bushes["topRightBlockTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level1Bushes["lowerTopLeftBlockTile"].GetPosition().x &&
        position.x <= level1Bushes["lowerTopLeftBlockTile"].GetPosition().x + level1Bushes["lowerTopLeftBlockTile"].GetRectangle().width &&
        position.y + rectangle.height >= level1Bushes["lowerTopLeftBlockTile"].GetPosition().y &&
        position.y <= level1Bushes["lowerTopLeftBlockTile"].GetPosition().y + level1Bushes["lowerTopLeftBlockTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level1Bushes["lowerTopRightBlockTile"].GetPosition().x &&
        position.x <= level1Bushes["lowerTopRightBlockTile"].GetPosition().x + level1Bushes["lowerTopRightBlockTile"].GetRectangle().width &&
        position.y + rectangle.height >= level1Bushes["lowerTopRightBlockTile"].GetPosition().y &&
        position.y <= level1Bushes["lowerTopRightBlockTile"].GetPosition().y + level1Bushes["lowerTopRightBlockTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level1Bushes["topBottomLeftBlockTile"].GetPosition().x &&
        position.x <= level1Bushes["topBottomLeftBlockTile"].GetPosition().x + level1Bushes["topBottomLeftBlockTile"].GetRectangle().width &&
        position.y + rectangle.height >= level1Bushes["topBottomLeftBlockTile"].GetPosition().y &&
        position.y <= level1Bushes["topBottomLeftBlockTile"].GetPosition().y + level1Bushes["topBottomLeftBlockTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level1Bushes["topBottomRightBlockTile"].GetPosition().x &&
        position.x <= level1Bushes["topBottomRightBlockTile"].GetPosition().x + level1Bushes["topBottomRightBlockTile"].GetRectangle().width &&
        position.y + rectangle.height >= level1Bushes["topBottomRightBlockTile"].GetPosition().y &&
        position.y <= level1Bushes["topBottomRightBlockTile"].GetPosition().y + level1Bushes["topBottomRightBlockTile"].GetRectangle().height)
    {
        if (position != lastPosition) position = lastPosition;
        if (center != lastPosition) center = { lastPosition.x + 40.0f, lastPosition.y + 10.0f };
    }

    else
    {
        if (lastPosition != position) lastPosition = position;
    }
}

void Player::CheckForLevel2BushCollision()
{
    if (position.x + rectangle.width >= level2Bushes["leftBushTile"].GetPosition().x &&
        position.x <= level2Bushes["leftBushTile"].GetPosition().x + level2Bushes["leftBushTile"].GetRectangle().width + 10.0f &&
        position.y + rectangle.height >= level2Bushes["leftBushTile"].GetPosition().y &&
        position.y <= level2Bushes["leftBushTile"].GetPosition().y + level2Bushes["leftBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level2Bushes["rightBushTile"].GetPosition().x &&
        position.x <= level2Bushes["rightBushTile"].GetPosition().x + level2Bushes["rightBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= level2Bushes["rightBushTile"].GetPosition().y &&
        position.y <= level2Bushes["rightBushTile"].GetPosition().y + level2Bushes["rightBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level2Bushes["topLeftBushTile"].GetPosition().x - 100.0f &&
        position.x <= level2Bushes["topLeftBushTile"].GetPosition().x + level2Bushes["topLeftBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= level2Bushes["topLeftBushTile"].GetPosition().y &&
        position.y <= level2Bushes["topLeftBushTile"].GetPosition().y + level2Bushes["topLeftBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level2Bushes["topRightBushTile"].GetPosition().x - 100.0f &&
        position.x <= level2Bushes["topRightBushTile"].GetPosition().x + level2Bushes["topRightBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= level2Bushes["topRightBushTile"].GetPosition().y &&
        position.y <= level2Bushes["topRightBushTile"].GetPosition().y + level2Bushes["topRightBushTile"].GetRectangle().height
        ||
        position.x + rectangle.width >= level2Bushes["bottomBushTile"].GetPosition().x - 100.0f &&
        position.x <= level2Bushes["bottomBushTile"].GetPosition().x + level2Bushes["bottomBushTile"].GetRectangle().width &&
        position.y + rectangle.height >= level2Bushes["bottomBushTile"].GetPosition().y &&
        position.y <= level2Bushes["bottomBushTile"].GetPosition().y + level2Bushes["bottomBushTile"].GetRectangle().height)
    {
        if (position != lastPosition) position = lastPosition;
        if (center != lastPosition) center = { lastPosition.x + 40.0f, lastPosition.y + 10.0f };
    }

    else
    {
        if (lastPosition != position) lastPosition = position;
    }
}

void Player::CheckForLevel1BushCollision(Vector2& position_, Vector2& lastPosition_, Rectangle& rectangle_)
{
    // Check if any of the characters/objects collided with any of the level1Bushes
    if (position_.x + rectangle_.width >= level1Bushes["topBushTile"].GetPosition().x &&
        position_.x <= level1Bushes["topBushTile"].GetPosition().x + level1Bushes["topBushTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level1Bushes["topBushTile"].GetPosition().y &&
        position_.y <= level1Bushes["topBushTile"].GetPosition().y + level1Bushes["topBushTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level1Bushes["topLeftBushTile"].GetPosition().x &&
        position_.x <= level1Bushes["topLeftBushTile"].GetPosition().x + level1Bushes["topLeftBushTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level1Bushes["topLeftBushTile"].GetPosition().y &&
        position_.y <= level1Bushes["topLeftBushTile"].GetPosition().y + level1Bushes["topLeftBushTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level1Bushes["topRightBushTile"].GetPosition().x &&
        position_.x <= level1Bushes["topRightBushTile"].GetPosition().x + level1Bushes["topRightBushTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level1Bushes["topRightBushTile"].GetPosition().y &&
        position_.y <= level1Bushes["topRightBushTile"].GetPosition().y + level1Bushes["topRightBushTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level1Bushes["lowerTopLeftBushTile"].GetPosition().x &&
        position_.x <= level1Bushes["lowerTopLeftBushTile"].GetPosition().x + level1Bushes["lowerTopLeftBushTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level1Bushes["lowerTopLeftBushTile"].GetPosition().y &&
        position_.y <= level1Bushes["lowerTopLeftBushTile"].GetPosition().y + level1Bushes["lowerTopLeftBushTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level1Bushes["lowerTopRightBushTile"].GetPosition().x &&
        position_.x <= level1Bushes["lowerTopRightBushTile"].GetPosition().x + level1Bushes["lowerTopRightBushTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level1Bushes["lowerTopRightBushTile"].GetPosition().y &&
        position_.y <= level1Bushes["lowerTopRightBushTile"].GetPosition().y + level1Bushes["lowerTopRightBushTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level1Bushes["middleLeftBushTile"].GetPosition().x &&
        position_.x <= level1Bushes["middleLeftBushTile"].GetPosition().x + level1Bushes["middleLeftBushTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level1Bushes["middleLeftBushTile"].GetPosition().y &&
        position_.y <= level1Bushes["middleLeftBushTile"].GetPosition().y + level1Bushes["middleLeftBushTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level1Bushes["middleRightBushTile"].GetPosition().x &&
        position_.x <= level1Bushes["middleRightBushTile"].GetPosition().x + level1Bushes["middleRightBushTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level1Bushes["middleRightBushTile"].GetPosition().y &&
        position_.y <= level1Bushes["middleRightBushTile"].GetPosition().y + level1Bushes["middleRightBushTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level1Bushes["topBottomLeftBushTile"].GetPosition().x &&
        position_.x <= level1Bushes["topBottomLeftBushTile"].GetPosition().x + level1Bushes["topBottomLeftBushTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level1Bushes["topBottomLeftBushTile"].GetPosition().y &&
        position_.y <= level1Bushes["topBottomLeftBushTile"].GetPosition().y + level1Bushes["topBottomLeftBushTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level1Bushes["topBottomRightBushTile"].GetPosition().x &&
        position_.x <= level1Bushes["topBottomRightBushTile"].GetPosition().x + level1Bushes["topBottomRightBushTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level1Bushes["topBottomRightBushTile"].GetPosition().y &&
        position_.y <= level1Bushes["topBottomRightBushTile"].GetPosition().y + level1Bushes["topBottomRightBushTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level1Bushes["bottomMiddleBushTile"].GetPosition().x &&
        position_.x <= level1Bushes["bottomMiddleBushTile"].GetPosition().x + level1Bushes["bottomMiddleBushTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level1Bushes["bottomMiddleBushTile"].GetPosition().y &&
        position_.y <= level1Bushes["bottomMiddleBushTile"].GetPosition().y + level1Bushes["bottomMiddleBushTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level1Bushes["topLeftBlockTile"].GetPosition().x &&
        position_.x <= level1Bushes["topLeftBlockTile"].GetPosition().x + level1Bushes["topLeftBlockTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level1Bushes["topLeftBlockTile"].GetPosition().y &&
        position_.y <= level1Bushes["topLeftBlockTile"].GetPosition().y + level1Bushes["topLeftBlockTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level1Bushes["topRightBlockTile"].GetPosition().x &&
        position_.x <= level1Bushes["topRightBlockTile"].GetPosition().x + level1Bushes["topRightBlockTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level1Bushes["topRightBlockTile"].GetPosition().y &&
        position_.y <= level1Bushes["topRightBlockTile"].GetPosition().y + level1Bushes["topRightBlockTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level1Bushes["lowerTopLeftBlockTile"].GetPosition().x &&
        position_.x <= level1Bushes["lowerTopLeftBlockTile"].GetPosition().x + level1Bushes["lowerTopLeftBlockTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level1Bushes["lowerTopLeftBlockTile"].GetPosition().y &&
        position_.y <= level1Bushes["lowerTopLeftBlockTile"].GetPosition().y + level1Bushes["lowerTopLeftBlockTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level1Bushes["lowerTopRightBlockTile"].GetPosition().x &&
        position_.x <= level1Bushes["lowerTopRightBlockTile"].GetPosition().x + level1Bushes["lowerTopRightBlockTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level1Bushes["lowerTopRightBlockTile"].GetPosition().y &&
        position_.y <= level1Bushes["lowerTopRightBlockTile"].GetPosition().y + level1Bushes["lowerTopRightBlockTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level1Bushes["topBottomLeftBlockTile"].GetPosition().x &&
        position_.x <= level1Bushes["topBottomLeftBlockTile"].GetPosition().x + level1Bushes["topBottomLeftBlockTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level1Bushes["topBottomLeftBlockTile"].GetPosition().y &&
        position_.y <= level1Bushes["topBottomLeftBlockTile"].GetPosition().y + level1Bushes["topBottomLeftBlockTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level1Bushes["topBottomRightBlockTile"].GetPosition().x &&
        position_.x <= level1Bushes["topBottomRightBlockTile"].GetPosition().x + level1Bushes["topBottomRightBlockTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level1Bushes["topBottomRightBlockTile"].GetPosition().y &&
        position_.y <= level1Bushes["topBottomRightBlockTile"].GetPosition().y + level1Bushes["topBottomRightBlockTile"].GetRectangle().height)
    {
        if (position_ != lastPosition_) position_ = lastPosition_;
    }

    else
    {
        if (lastPosition_ != position_) lastPosition_ = position_;
    }
}

void Player::CheckForLevel2BushCollision(Vector2& position_, Vector2& lastPosition_, Rectangle& rectangle_)
{
    // Check if any of the characters/objects collided with any of the level2Bushes
    if (position_.x + rectangle_.width >= level2Bushes["leftBushTile"].GetPosition().x &&
        position_.x <= level2Bushes["leftBushTile"].GetPosition().x + level2Bushes["leftBushTile"].GetRectangle().width + 10.0f &&
        position_.y + rectangle_.height >= level2Bushes["leftBushTile"].GetPosition().y &&
        position_.y <= level2Bushes["leftBushTile"].GetPosition().y + level2Bushes["leftBushTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level2Bushes["rightBushTile"].GetPosition().x &&
        position_.x <= level2Bushes["rightBushTile"].GetPosition().x + level2Bushes["rightBushTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level2Bushes["rightBushTile"].GetPosition().y &&
        position_.y <= level2Bushes["rightBushTile"].GetPosition().y + level2Bushes["rightBushTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level2Bushes["topLeftBushTile"].GetPosition().x - 100.0f &&
        position_.x <= level2Bushes["topLeftBushTile"].GetPosition().x + level2Bushes["topLeftBushTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level2Bushes["topLeftBushTile"].GetPosition().y &&
        position_.y <= level2Bushes["topLeftBushTile"].GetPosition().y + level2Bushes["topLeftBushTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level2Bushes["topRightBushTile"].GetPosition().x - 100.0f &&
        position_.x <= level2Bushes["topRightBushTile"].GetPosition().x + level2Bushes["topRightBushTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level2Bushes["topRightBushTile"].GetPosition().y &&
        position_.y <= level2Bushes["topRightBushTile"].GetPosition().y + level2Bushes["topRightBushTile"].GetRectangle().height
        ||
        position_.x + rectangle_.width >= level2Bushes["bottomBushTile"].GetPosition().x - 100.0f &&
        position_.x <= level2Bushes["bottomBushTile"].GetPosition().x + level2Bushes["bottomBushTile"].GetRectangle().width &&
        position_.y + rectangle_.height >= level2Bushes["bottomBushTile"].GetPosition().y &&
        position_.y <= level2Bushes["bottomBushTile"].GetPosition().y + level2Bushes["bottomBushTile"].GetRectangle().height)
    {
        if (position_ != lastPosition_) position_ = lastPosition_;
    }

    else
    {
        if (lastPosition_ != position_) lastPosition_ = position_;
    }
}