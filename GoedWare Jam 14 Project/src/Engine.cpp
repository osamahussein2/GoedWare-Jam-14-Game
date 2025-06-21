#include "Engine.h"
#include "Window.h"
#include "GameObject.h"
#include "NoiseBar.h"
#include "Player.h"
#include "Timer.h"
#include "Audio.h"
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
    InitAudioDevice();

    std::unordered_map<std::string, GameObject> sprites;

    Timer timer;

    MusicAudio trackDemo;

    sprites["Player"].InitializeSprite("Sprite1");
    sprites["Player2"].InitializeSprite("Sprite2");

    Player::Instance()->InitializeCharacter();

    timer.InitializeTimer();

    trackDemo.InitializeMusic("TrackDemo");

    Window::Instance()->SetFPS();

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update

        // Draw
        BeginDrawing();

        ClearBackground(BLACK);

        Player::Instance()->BeginFollowPlayerCamera();

        sprites["Player"].DrawSprite(Player::Instance()->GetCenter(), Player::Instance()->GetRadius(), WHITE, 
            Player::Instance()->GetLightOn());

        sprites["Player2"].DrawSprite(Player::Instance()->GetCenter(), Player::Instance()->GetRadius(), WHITE,
            Player::Instance()->GetLightOn());

        Player::Instance()->DrawCharacter();

        Player::Instance()->DrawUI();
        
        trackDemo.PlayMusic(true);

        timer.RenderTimer(WHITE);

        EndDrawing();
    }

    sprites["Player"].UnloadSprite();
    sprites["Player2"].UnloadSprite();
    Player::Instance()->UnloadCharacter();
    trackDemo.UnloadMusic();
    CloseWindow();
}