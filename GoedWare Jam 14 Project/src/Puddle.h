#ifndef PUDDLE_H
#define PUDDLE_H

#include "GameObject.h"

class Puddle : public GameObject
{
public:
	Puddle();
	~Puddle();

	virtual void InitializeGameObject(std::string childNode2_);
	virtual void DrawSprite(Color color_);
	virtual void UnloadSprite();

	void ResetPuddle(std::string childNode2_);

	bool GetPlayerSteppedOnPuddle() const { return playerSteppedOnPuddle; }

private:
	static constexpr float updateTime{ 1.0f / 12.0f };
	float runningTime;

	int totalFramesX;
	int xFrame;

	bool playerSteppedOnPuddle;
};

#endif