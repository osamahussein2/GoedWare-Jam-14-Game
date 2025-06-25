#include "Timer.h"
#include "Window.h"
#include "Player.h"

Timer::Timer() : textFile("XML files/Text.xml"), rootNode(), minutes(), seconds()
{
}

Timer::~Timer()
{
}

void Timer::InitializeTimer()
{
    document.parse<0>(textFile.data());

    rootNode = document.first_node("Text");

    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("TextInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node("TimerText"); fileNode2;
            fileNode2 = fileNode->next_sibling())
        {
            minutes = atoi(fileNode2->first_attribute("minutes")->value());
            seconds = atof(fileNode2->first_attribute("seconds")->value());
        }
    }
}

void Timer::ResetTimer()
{
    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("TextInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node("TimerText"); fileNode2;
            fileNode2 = fileNode->next_sibling())
        {
            minutes = atoi(fileNode2->first_attribute("minutes")->value());
            seconds = atof(fileNode2->first_attribute("seconds")->value());
        }
    }
}

void Timer::RenderTimer(Color color)
{
    // Make sure the seconds only decrement when it's greater than 0
    if (seconds > 0 && Player::Instance()->GetPlayerInputEnabled()) seconds -= Window::Instance()->GetDeltaTime();

    // If there are minutes left with 0 seconds, reset the seconds timer and decrease minutes by 1 each time
    if (minutes > 0 && seconds <= 0.0f && Player::Instance()->GetPlayerInputEnabled())
    {
        minutes--;
        seconds = 59.99f;
    }

    // If timer has expired, transition to the game over state
    if (minutes <= 0 && seconds <= 0.0f)
    {
        Player::Instance()->SetHasFailed(true);
    }

    if (seconds >= 60.0f && Player::Instance()->GetPlayerInputEnabled())
    {
        seconds = 59.99f; // Clamp seconds to 59.99f if it's greater than that
    }

    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("TextInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node("TimerText"); fileNode2;
            fileNode2 = fileNode->next_sibling())
        {
            if (seconds < 10.0f)
            {
                DrawText
                (
                    (std::to_string(minutes) + ":0" + std::to_string(static_cast<int>(seconds))).c_str(),
                    atoi(fileNode2->first_attribute("textX")->value()),
                    atoi(fileNode2->first_attribute("textY")->value()),
                    atoi(fileNode2->first_attribute("fontSize")->value()), color
                );
            }

            else
            {
                DrawText
                (
                    (std::to_string(minutes) + ":" + std::to_string(static_cast<int>(seconds))).c_str(),
                    atoi(fileNode2->first_attribute("textX")->value()),
                    atoi(fileNode2->first_attribute("textY")->value()),
                    atoi(fileNode2->first_attribute("fontSize")->value()), color
                );
            }
        }
    }
}