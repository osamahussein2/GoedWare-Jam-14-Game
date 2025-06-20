#ifndef WINDOW_H
#define WINDOW_H

#include "raylib.h"
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#include <string>

class Window
{
public:
	Window();
	~Window();

	static std::shared_ptr<Window> Instance();

	void InitializeRaylibWindow();
	void SetFPS();
	
	int GetWindowWidth() const { return windowWidth; }
	int GetWindowHeight() const { return windowHeight; }

	const float GetDeltaTime();

private:
	rapidxml::file<> windowFile;
	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* rootNode;

	int windowWidth, windowHeight;

	static std::shared_ptr<Window> windowInstance;
};

#endif