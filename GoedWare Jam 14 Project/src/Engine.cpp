#include "Engine.h"
#include "Window.h"
#include "GameObject.h"
#include "NoiseBar.h"
#include "Player.h"
#include "Timer.h"
#include "Audio.h"
#include "World.h"
#include "Text.h"
#include <array>

std::shared_ptr<Engine> Engine::engineInstance = nullptr;

MusicAudio trackDemo;
Text gameOverText, levelCompleteText;

std::array<World, 2> levels;

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

    levels[0].InitializeWorld("WorldBackground");
    levels[1].InitializeWorld("WorldBackground2");

    Player::Instance()->InitializeCharacter();

    trackDemo.InitializeMusic("TrackDemo");
    trackDemo.SetMusicAudioVolume(0.2f);

    gameOverText.InitializeText();
    levelCompleteText.InitializeText();

    Window::Instance()->SetFPS();

    float gameOverTime{};
    float levelCompleteTime{};

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
            if (!Player::Instance()->HasCompletedLevel())
            {
                // Update player camera and logic
                Player::Instance()->BeginFollowPlayerCamera();

                if (Player::Instance()->GetLevelNumber() == 1) // Level 1
                {
                    levels[0].DrawWorld(Player::Instance()->GetLightOn(), Color{ 255, 255, 255, 150 });
                    Player::Instance()->DrawGameObjects();
                }

                else if (Player::Instance()->GetLevelNumber() == 2) // Level 2
                {
                    levels[1].DrawWorld(Player::Instance()->GetLightOn(), Color{ 255, 255, 255, 150 });
                    Player::Instance()->DrawGameObjects();
                }

                Player::Instance()->DrawCharacter();
                Player::Instance()->DrawUI();
            }

            else if (Player::Instance()->HasCompletedLevel())
            {
                levelCompleteText.RenderText("LevelCompleteText", GREEN);

                levelCompleteTime += Window::Instance()->GetDeltaTime(); // Increment game over time

                // Reset game state
                if (levelCompleteTime >= 2.0f)
                {
                    Player::Instance()->ResetCharacter();

                    levelCompleteTime = 0.0f;
                }
            }
        }

        else if (Player::Instance()->HasFailed()) // Otherwise, player failed the level
        {
            gameOverText.RenderText("GameOverText", RED);

            gameOverTime += Window::Instance()->GetDeltaTime(); // Increment game over time

            // Reset game state
            if (gameOverTime >= 2.0f)
            {
                Player::Instance()->ResetCharacter();

                gameOverTime = 0.0f;
            }
        }
        
        trackDemo.PlayMusic(true);

        EndDrawing();
    }

    for (int i = 0; i < levels.size(); i++) levels[i].UnloadWorld();

    Player::Instance()->UnloadCharacter();
    trackDemo.UnloadMusic();

    CloseWindow();
}