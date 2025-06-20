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

private:
	rapidxml::file<> windowFile;
	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* rootNode;

	static std::shared_ptr<Window> windowInstance;
};

#endif