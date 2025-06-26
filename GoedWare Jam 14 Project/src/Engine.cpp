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
#include <array>

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

    std::array<World, 2> levels;

    levels[0].InitializeWorld("WorldBackground");
    levels[1].InitializeWorld("WorldBackground2");

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

            if (Player::Instance()->GetLevelNumber() == 1) // Level 1
            {
                levels[0].DrawWorld(Player::Instance()->GetLightOn(), Color{ 255, 255, 255, 150 });
                Player::Instance()->DrawGameObjects();
                spectre.DrawCharacter();
            }

            else if (Player::Instance()->GetLevelNumber() == 2) // Level 2
            {
                levels[1].DrawWorld(Player::Instance()->GetLightOn(), Color{ 255, 255, 255, 150 });
                Player::Instance()->DrawGameObjects();
                spectre.DrawCharacter();
            }

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

    for (int i = 0; i < levels.size(); i++) levels[i].UnloadWorld();

    spectre.UnloadCharacter();

    Player::Instance()->UnloadCharacter();
    trackDemo.UnloadMusic();

    CloseWindow();
}