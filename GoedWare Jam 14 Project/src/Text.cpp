#include "Text.h"

Text::Text() : textFile("XML files/Text.xml")
{
}

Text::~Text()
{
}

void Text::InitializeText()
{
    document.parse<0>(textFile.data());

    rootNode = document.first_node("Text");
}

void Text::RenderText(std::string childNode2_, Color color)
{
    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("TextInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node(childNode2_.c_str()); fileNode2;
            fileNode2 = fileNode->next_sibling())
        {
            DrawText
            (
                fileNode2->first_attribute("text")->value(),
                atoi(fileNode2->first_attribute("textX")->value()),
                atoi(fileNode2->first_attribute("textY")->value()),
                atoi(fileNode2->first_attribute("fontSize")->value()), color
            );
        }
    }
}
