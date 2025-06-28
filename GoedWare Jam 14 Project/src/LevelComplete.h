#ifndef LEVELCOMPLETE_H
#define LEVELCOMPLETE_H

#include "MyRectangle.h"

class LevelComplete : public MyRectangle
{
public:
	LevelComplete();
	~LevelComplete();

	virtual void InitializeSprite(std::string childNode2_) override;
	virtual void DrawSprite(Color color_) override;

	Rectangle GetRectangle() { return rectangle; }
};

#endif