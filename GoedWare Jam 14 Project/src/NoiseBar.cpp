#include "NoiseBar.h"
#include "Window.h"

NoiseBar::NoiseBar() : currentNoise(), maxNoiseThreshold(), isNoiseIncreased()
{
}

NoiseBar::~NoiseBar()
{
}

void NoiseBar::InitializeSprite(const char* childNode2_)
{
    document.parse<0>(barFile.data());

    rootNode = document.first_node("Bar");

    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("BarInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node(childNode2_); fileNode2;
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
    if (isNoiseIncreased) currentNoise += 100.0f * Window::Instance()->GetDeltaTime();

    // Otherwise, decrement current noise value
    else currentNoise -= 100.0f * Window::Instance()->GetDeltaTime();

    // Clamp the current noise value to the max noise threshold
    if (currentNoise >= maxNoiseThreshold)
    {
        if (currentNoise != maxNoiseThreshold) currentNoise = maxNoiseThreshold;
    }
    // Otherwise clamp the current noise value to 0
    else if (currentNoise <= 0.0f) 
    { 
        if (currentNoise != 0.0f) currentNoise = 0.0f;
    }

    // For now, update noise increase boolean if space key is held
    if (IsKeyDown(KEY_SPACE)) isNoiseIncreased = true;
    else if (!IsKeyDown(KEY_SPACE)) isNoiseIncreased = false;

    // Set the rectangle's width to the current noise value
    if (rectangle.width != currentNoise) rectangle.width = currentNoise;
}
