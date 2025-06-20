#include "Player.h"
#include "Window.h"

std::shared_ptr<Player> Player::playerInstance = nullptr;

Player::Player()
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

    // Make camera follow the player around
    camera.target = { position.x, position.y };

    Vector2 velocity{};
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) velocity.y = -100.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) velocity.y = 100.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) velocity.x = -100.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) velocity.x = 100.0f;

    // Animate the player's facing direction according to last recorded velocity before stopping
    if (velocity.x < 0.0f) rectangle.width = -1.0f * static_cast<float>(texture.width) / totalFrames;
    else if (velocity.x > 0.0f) rectangle.width = 1.0f * static_cast<float>(texture.width) / totalFrames;

    // Move the player's position by using velocity and delta time
    position = Vector2Add(position, Vector2Scale(velocity, Window::Instance()->GetDeltaTime()));

    // Render the player
    DrawTextureRec(texture, rectangle, position, WHITE);

    EndMode2D();
}

void Player::UnloadCharacter()
{
    // Unload the player texture
    UnloadTexture(texture);
}
