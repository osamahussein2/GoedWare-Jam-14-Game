#include "Player.h"
#include "Window.h"
#include "World.h"

std::shared_ptr<Player> Player::playerInstance = nullptr;

const std::string fullTag = "Full";
const std::string currentTag = "Current";

Player::Player() : lightOn(false)
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
            totalFrames = atoi(fileNode2->first_attribute("totalFrames")->value());
            texture = LoadTexture(fileNode2->first_attribute("spritePath")->value());

            rectangle.width = static_cast<float>(texture.width / totalFrames);
            rectangle.height = static_cast<float>(texture.height);
            rectangle.x = 0.0f;
            rectangle.y = 0.0f;

            position = Vector2{ static_cast<float>(atof(fileNode2->first_attribute("posX")->value())),
                static_cast<float>(atof(fileNode2->first_attribute("posY")->value())) };
        }
    }

    camera.target = { position.x, position.y };
    camera.offset = { Window::Instance()->GetWindowWidth() / 2.25f, Window::Instance()->GetWindowHeight() / 2.0f };
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;

    center = { position.x + 40.0f, position.y + 10.0f };

    noiseBars[fullTag].InitializeSprite("FullNoiseBar");
    noiseBars[currentTag].InitializeCurrentBar(currentTag);
}

void Player::BeginFollowPlayerCamera()
{
    BeginMode2D(camera);
}

void Player::DrawCharacter()
{
    // Increment running time for checking if it reaches update animation time
    runningTime += Window::Instance()->GetDeltaTime();

    if (runningTime >= updateTime)
    {
        runningTime = 0.0f;

        // Update animation frame
        rectangle.x = rectangle.width * (frame % totalFrames);
        frame++;
    }

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

    Vector2 velocity{};
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
    {
        if (noiseBars[currentTag].isNoiseIncreased != true) noiseBars[currentTag].isNoiseIncreased = true;

        if (position.y >= World::Instance()->GetPosition().y) velocity.y = -100.0f;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
    {
        if (noiseBars[currentTag].isNoiseIncreased != true) noiseBars[currentTag].isNoiseIncreased = true;

        if (position.y <= World::Instance()->GetRectangle().height) velocity.y = 100.0f;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    {
        if (noiseBars[currentTag].isNoiseIncreased != true) noiseBars[currentTag].isNoiseIncreased = true;

        if (position.x >= World::Instance()->GetPosition().x) velocity.x = -100.0f;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
    {
        if (noiseBars[currentTag].isNoiseIncreased != true) noiseBars[currentTag].isNoiseIncreased = true;

        if (position.x <= World::Instance()->GetRectangle().width) velocity.x = 100.0f;
    }

    else if (!IsKeyDown(KEY_W) && !IsKeyDown(KEY_UP) && !IsKeyDown(KEY_S) && !IsKeyDown(KEY_DOWN) &&
        !IsKeyDown(KEY_A) && !IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_RIGHT))
    {
        if (noiseBars[currentTag].isNoiseIncreased != false) noiseBars[currentTag].isNoiseIncreased = false;
    }

    if (IsKeyPressed(KEY_F)) lightOn = !lightOn;

    // If light isn't on, draw a black rectangle to give the illusion that lights are off
    if (!lightOn)
    {
        if (velocity.x == 0.0f && velocity.y == 0.0f)
        {
            if (noiseBars[currentTag].isNoiseIncreased != false) noiseBars[currentTag].isNoiseIncreased = false;
        }
    }

    // But if light is on, draw a slightly transparent black rectangle to give the illusion that lights are on
    else if (lightOn)
    {
        // Draw circle as a light for the player
        DrawCircleGradient(center.x, center.y, circleRadius, Color{ 255, 255, 255, 120 }, Color{ 255, 255, 255, 10 });

        if (noiseBars[currentTag].isNoiseIncreased != true) noiseBars[currentTag].isNoiseIncreased = true;
    }

    // Animate the player's facing direction according to last recorded velocity before stopping
    if (velocity.x < 0.0f) rectangle.width = -1.0f * static_cast<float>(texture.width) / totalFrames;
    else if (velocity.x > 0.0f) rectangle.width = 1.0f * static_cast<float>(texture.width) / totalFrames;

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
}

void Player::DrawUI()
{
    noiseBars[fullTag].UpdateCurrentNoise(fullTag);
    noiseBars[currentTag].UpdateCurrentNoise(currentTag);

    noiseBars[fullTag].DrawSprite(Color{ 125, 125, 125, 255 });
    noiseBars[currentTag].DrawSprite(RED);
}
