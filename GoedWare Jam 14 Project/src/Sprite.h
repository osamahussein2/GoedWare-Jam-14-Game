#ifndef SPRITE_H
#define SPRITE_H

#include <raylib.h>
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

class Sprite
{
public:
	Sprite();
	~Sprite();

	void InitializeSprite(const char* rootChildNode_, const char* childNode2_);
	void DrawSprite(Color color_);
	void UnloadSprite();

private:
	rapidxml::file<> spriteFile;
	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* rootNode;

	Texture2D texture;
	Rectangle rectangle;

	Vector2 position;
};

#endif