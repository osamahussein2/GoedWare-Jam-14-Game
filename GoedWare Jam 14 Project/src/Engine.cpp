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
    std::unordered_map<std::string, NoiseBar> noiseBars;

    Timer timer;

    MusicAudio trackDemo;

    sprites["Player"].InitializeSprite("Sprite1");
    sprites["Player2"].InitializeSprite("Sprite2");

    Player::Instance()->InitializeCharacter();

    const std::string fullTag = "Full";
    const std::string currentTag = "Current";

    noiseBars[fullTag].InitializeSprite("FullNoiseBar");
    noiseBars[currentTag].InitializeCurrentBar(currentTag);

    timer.InitializeTimer();

    trackDemo.InitializeMusic("TrackDemo");

    bool lightOn = false;

    Window::Instance()->SetFPS();

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update

        // Draw
        BeginDrawing();

        ClearBackground(BLACK);

        Player::Instance()->BeginFollowPlayerCamera();

        sprites["Player"].DrawSprite(WHITE);
        sprites["Player2"].DrawSprite(WHITE);

        Player::Instance()->DrawCharacter();

        noiseBars[fullTag].UpdateCurrentNoise(fullTag);
        noiseBars[currentTag].UpdateCurrentNoise(currentTag);

        if (IsKeyPressed(KEY_F)) lightOn = !lightOn;

        // If light isn't on, draw a black rectangle to give the illusion that lights are off
        if (!lightOn)
        {
            DrawRectangle(0, 0, Window::Instance()->GetWindowWidth(), Window::Instance()->GetWindowHeight(), BLACK);

            if (noiseBars[currentTag].isNoiseIncreased != false) noiseBars[currentTag].isNoiseIncreased = false;
        }

        // But if light is on, draw a slightly transparent black rectangle to give the illusion that lights are on
        else if (lightOn)
        {
            const float circleRadius = 250.0f;

            // Draw left side of rectangle for left view
            DrawRectangle(0, 0, (Window::Instance()->GetWindowWidth() / 2.0f) - circleRadius, 
                Window::Instance()->GetWindowHeight(), BLACK);

            // Draw top of rectangle until it reaches the top of circle
            DrawRectangle((Window::Instance()->GetWindowWidth() / 2.0f) - circleRadius, 0, circleRadius * 2,
                (Window::Instance()->GetWindowHeight() / 2.0f) - circleRadius, BLACK);

            // Draw right side of rectangle for right view
            DrawRectangle((Window::Instance()->GetWindowWidth() / 2.0f) + circleRadius, 0.0f, 
                (Window::Instance()->GetWindowWidth() / 2.0f) - circleRadius, Window::Instance()->GetWindowHeight(), BLACK);

            // Draw bottom of rectangle until it reaches the bottom of circle
            DrawRectangle((Window::Instance()->GetWindowWidth() / 2.0f) - circleRadius, 
                (Window::Instance()->GetWindowHeight() / 2.0f) + circleRadius, circleRadius * 2,
                (Window::Instance()->GetWindowHeight() / 2.0f) - circleRadius, BLACK);

            DrawCircle(Window::Instance()->GetWindowWidth() / 2.0f, Window::Instance()->GetWindowHeight() / 2.0f,
                circleRadius, Color{ 255, 255, 255, 50 });

            if (noiseBars[currentTag].isNoiseIncreased != true) noiseBars[currentTag].isNoiseIncreased = true;
        }

        noiseBars[fullTag].DrawSprite(Color{ 125, 125, 125, 255 });
        noiseBars[currentTag].DrawSprite(RED);
        
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