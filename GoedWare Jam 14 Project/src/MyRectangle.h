#ifndef MYRECTANGLE_H
#define MYRECTANGLE_H

#include <raylib.h>
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

class MyRectangle
{
public:
	MyRectangle() : barFile("XML files/Bar.xml"), rootNode(), rectangle() {}
	virtual ~MyRectangle() {}

	virtual void InitializeSprite(std::string childNode2_) = 0;
	virtual void DrawSprite(Color color_) = 0;

protected:
	rapidxml::file<> barFile;
	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* rootNode;

	Rectangle rectangle;
};

#endif
