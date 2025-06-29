#ifndef TIMER_H
#define TIMER_H

#include <raylib.h>
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

class Timer
{
public:
	Timer();
	~Timer();

	void InitializeTimer();
	void ResetTimer();
	void RenderTimer(Color color);

private:
	rapidxml::file<> textFile;
	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* rootNode;

	int minutes;
	float seconds;
};

#endif