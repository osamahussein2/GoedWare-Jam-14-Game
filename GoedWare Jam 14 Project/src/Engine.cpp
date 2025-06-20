#include "Engine.h"
#include "Window.h"
#include "GameObject.h"
#include "NoiseBar.h"
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

    std::unordered_map<std::string, GameObject> sprites;
    std::unordered_map<std::string, NoiseBar> noiseBars;

    sprites["Player"].InitializeSprite("Sprite1");
    sprites["Player2"].InitializeSprite("Sprite2");

    Player::Instance()->InitializeCharacter();

    const std::string fullTag = "Full";
    const std::string currentTag = "Current";

    noiseBars[fullTag].InitializeSprite("FullNoiseBar");
    noiseBars[currentTag].InitializeCurrentBar(currentTag);

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

        noiseBars[fullTag].UpdateCurrentNoise(fullTag);
        noiseBars[currentTag].UpdateCurrentNoise(currentTag);

        noiseBars[fullTag].DrawSprite(BLACK);
        noiseBars[currentTag].DrawSprite(RED);

        EndDrawing();
    }

    sprites["Player"].UnloadSprite();
    sprites["Player2"].UnloadSprite();
    Player::Instance()->UnloadCharacter();
    CloseWindow();
}