#include "Engine.h"
#include "Window.h"
#include "GameObject.h"
#include "NoiseBar.h"
#include "Player.h"
#include "Timer.h"
#include "Audio.h"
#include "World.h"
#include "Spectre.h"
#include "Text.h"

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

    Spectre spectre;
    MusicAudio trackDemo;
    Text gameOverText;

    World::Instance()->InitializeWorld("WorldBackground");

    spectre.InitializeCharacter("Monster");

    Player::Instance()->InitializeCharacter();

    trackDemo.InitializeMusic("TrackDemo");
    trackDemo.SetMusicAudioVolume(0.2f);

    gameOverText.InitializeText();

    Window::Instance()->SetFPS();

    float gameOverTime{};

    // Main game loop
    while (!WindowShouldClose())
    {
        // Close the window after pressing Escape key
        if (IsKeyPressed(KEY_ESCAPE))
        {
            CloseWindow();
            break;
        }

        // Draw
        BeginDrawing();

        ClearBackground(BLACK);

        if (!Player::Instance()->HasFailed()) // Player hasn't failed the level yet
        {
            // Update player camera and logic
            Player::Instance()->BeginFollowPlayerCamera();

            World::Instance()->DrawWorld(Player::Instance()->GetLightOn(), Color{ 255, 255, 255, 150 });

            spectre.DrawCharacter();

            Player::Instance()->DrawCharacter();
            Player::Instance()->DrawUI();
        }

        else if (Player::Instance()->HasFailed()) // Otherwise, player failed the level
        {
            gameOverText.RenderText("GameOverText", RED);

            gameOverTime += Window::Instance()->GetDeltaTime(); // Increment game over time

            // Reset game state
            if (gameOverTime >= 2.0f)
            {
                spectre.ResetCharacter("Monster");
                Player::Instance()->ResetCharacter();

                gameOverTime = 0.0f;
            }
        }
        
        trackDemo.PlayMusic(true);

        EndDrawing();
    }

    World::Instance()->UnloadWorld();

    spectre.UnloadCharacter();

    Player::Instance()->UnloadCharacter();
    trackDemo.UnloadMusic();

    CloseWindow();
}