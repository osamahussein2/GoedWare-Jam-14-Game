#ifndef CHARACTER_H
#define CHARACTER_H

#include "raylib.h"
#include "raymath.h"
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

class Character
{
public:
	Character() : xFrame(), position(), texture(), rectangle(), totalFramesX(), characterFile("XML files/Character.xml"), 
	rootNode(), yFrame(), totalFramesY() {}
	virtual ~Character() {}

	virtual void InitializeCharacter() = 0;
	virtual void DrawCharacter() = 0;
	virtual void UnloadCharacter() = 0;

protected:
	rapidxml::file<> characterFile;
	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* rootNode;

	Texture2D texture;
	Rectangle rectangle;

	Vector2 position;

	float runningTime{};

	int totalFramesX, totalFramesY;
	int xFrame, yFrame;
};

#endif