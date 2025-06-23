#include "NoiseBar.h"
#include "Window.h"
#include "Player.h"

NoiseBar::NoiseBar() : currentNoise(), maxNoiseThreshold(), isNoiseIncreased(), noiseMaxedOut()
{
}

NoiseBar::~NoiseBar()
{
}

void NoiseBar::InitializeSprite(std::string childNode2_)
{
    document.parse<0>(barFile.data());

    rootNode = document.first_node("Bar");

    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("BarInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node(childNode2_.c_str()); fileNode2;
            fileNode2 = fileNode->next_sibling())
        {
            rectangle = 
            { 
                static_cast<float>(atof(fileNode2->first_attribute("posX")->value())),
                static_cast<float>(atof(fileNode2->first_attribute("posY")->value())),
                static_cast<float>(atof(fileNode2->first_attribute("recWidth")->value())),
                static_cast<float>(atof(fileNode2->first_attribute("recHeight")->value()))
            };
        }
    }
}

void NoiseBar::InitializeCurrentBar(std::string tag_)
{
    // Don't do anything and print tag isn't set to current
    if (tag_ != "Current")
    {
        printf("Tag isn't set to Current");
        return;
    }

    // Else if the tag is set to current, parse from the bar document to set the rectangle values
    else if (tag_ == "Current")
    {
        document.parse<0>(barFile.data());

        rootNode = document.first_node("Bar");

        for (rapidxml::xml_node<>* fileNode = rootNode->first_node("BarInfo"); fileNode;
            fileNode = fileNode->next_sibling())
        {
            for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node("FullNoiseBar"); fileNode2;
                fileNode2 = fileNode->next_sibling())
            {
                rectangle =
                {
                    static_cast<float>(atof(fileNode2->first_attribute("posX")->value())),
                    static_cast<float>(atof(fileNode2->first_attribute("posY")->value())),
                    static_cast<float>(atof(fileNode2->first_attribute("recWidth")->value())),
                    static_cast<float>(atof(fileNode2->first_attribute("recHeight")->value()))
                };

                maxNoiseThreshold = static_cast<float>(atof(fileNode2->first_attribute("recWidth")->value()));
            }
        }

        if (currentNoise != 0) currentNoise = 0;
    }
}

void NoiseBar::DrawSprite(Color color_)
{
	DrawRectangleRec(rectangle, color_);
}

void NoiseBar::UpdateCurrentNoise(std::string tag_)
{
    // Don't do anything if the tag isn't current
    if (tag_ != "Current") return;

    // Increment current noise value
    if (isNoiseIncreased && !noiseMaxedOut) currentNoise += 50.0f * Window::Instance()->GetDeltaTime();

    // Otherwise, decrement current noise value
    else if (!isNoiseIncreased && !noiseMaxedOut)
    {
        if (currentNoise > 0.0f) currentNoise -= 50.0f * Window::Instance()->GetDeltaTime();
    }

    // Set noise maxed out bool to true once current noise reaches the max noise threshold value
    if (currentNoise >= maxNoiseThreshold && !noiseMaxedOut) noiseMaxedOut = true;

    // Set the rectangle's width to the current noise value
    if (rectangle.width != currentNoise) rectangle.width = currentNoise;
}

void NoiseBar::ResetCurrentNoiseValue()
{
    // Reset current noise logic
    currentNoise = 0.0f;
    noiseMaxedOut = false;
}
