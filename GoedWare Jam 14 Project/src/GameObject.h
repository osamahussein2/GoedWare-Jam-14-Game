#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <raylib.h>
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

class GameObject
{
public:
	GameObject();
	~GameObject();

	void InitializeSprite(const char* childNode2_);
	void DrawSprite(Vector2 center_, float radius_, Color color_, bool lightOn_);
	void UnloadSprite();

	Rectangle GetRectangle() { return rectangle; }

private:
	rapidxml::file<> spriteFile;
	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* rootNode;

	Texture2D texture;
	Rectangle rectangle;

	Vector2 position;

	bool insideLight;
};

#endif