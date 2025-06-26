#ifndef BRANCH_H
#define BRANCH_H

#include "GameObject.h"

class Branch : public GameObject
{
public:
	Branch();
	~Branch();

	virtual void InitializeGameObject(std::string childNode2_);
	virtual void DrawSprite(Color color_);
	virtual void UnloadSprite();

	void ResetBranch(std::string childNode2_);

	bool GetPlayerSteppedOnBranch() const { return playerSteppedOnBranch; }

private:
	static constexpr float updateTime{ 1.0f / 12.0f };
	float runningTime;

	int totalFramesX;
	int xFrame;

	bool playerSteppedOnBranch;
};

#endif