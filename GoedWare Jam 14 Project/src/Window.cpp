#include "Window.h"

std::shared_ptr<Window> Window::windowInstance = nullptr;

Window::Window() : windowFile("XML files/Window.xml"), rootNode(), windowWidth(), windowHeight()
{
}

Window::~Window()
{
}

std::shared_ptr<Window> Window::Instance()
{
    if (windowInstance == nullptr)
    {
        windowInstance = std::make_shared<Window>();
        return windowInstance;
    }

    return windowInstance;
}

void Window::InitializeRaylibWindow()
{
    document.parse<0>(windowFile.data());

    rootNode = document.first_node("Window");

    // Iterate through all the root nodes in the xml file
    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("WindowInfo"); fileNode; fileNode = fileNode->next_sibling())
    {
        InitWindow
        (
            atoi(fileNode->first_attribute("width")->value()), // width
            atoi(fileNode->first_attribute("height")->value()), // height
            fileNode->first_attribute("name")->value() // window name
        );

        windowWidth = atoi(fileNode->first_attribute("width")->value());
        windowHeight = atoi(fileNode->first_attribute("height")->value());
    }
}

void Window::SetFPS()
{
    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("WindowInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node("SetFPS"); fileNode2;
            fileNode2 = fileNode->next_sibling())
        {
            SetTargetFPS(atoi(fileNode2->first_attribute("frameRate")->value()));
        }
    }
}

const float Window::GetDeltaTime()
{
    return GetFrameTime();
}
