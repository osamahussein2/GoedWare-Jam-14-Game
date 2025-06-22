#ifndef TEXT_H
#define TEXT_H

#include <raylib.h>
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

class Text
{
public:
	Text();
	~Text();

	void InitializeText();
	void RenderText(std::string childNode2_, Color color);

private:
	rapidxml::file<> textFile;
	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* rootNode;
};

#endif