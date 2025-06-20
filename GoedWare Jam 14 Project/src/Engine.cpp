#include "Engine.h"
#include "Window.h"
#include "Sprite.h"
#include "Player.h"
#include <unordered_map>

std::shared_ptr<Engine> Engine::engineInstance = nullptr;

Engine::Engine()
{
}

Engine::~Engine()
{
}

std::shared_ptr<Engine> Engine::Instance()
{
    if (engineInstance == nullptr)
    {
        engineInstance = std::make_shared<Engine>();
        return engineInstance;
    }

    return engineInstance;
}

void Engine::RunEngine()
{
    Window::Instance()->InitializeRaylibWindow();

    std::unordered_map<std::string, Sprite> sprites;

    sprites["Player"].InitializeSprite("SpriteInfo", "Sprite1");
    sprites["Player2"].InitializeSprite("SpriteInfo", "Sprite2");

    Player::Instance()->InitializeCharacter();

    Window::Instance()->SetFPS();

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update

        // Draw
        BeginDrawing();

        ClearBackground(WHITE);

        Player::Instance()->BeginFollowPlayerCamera();

        sprites["Player"].DrawSprite(WHITE);
        sprites["Player2"].DrawSprite(WHITE);

        Player::Instance()->DrawCharacter();

        EndDrawing();
    }

    sprites["Player"].UnloadSprite();
    sprites["Player2"].UnloadSprite();
    Player::Instance()->UnloadCharacter();
    CloseWindow();
}