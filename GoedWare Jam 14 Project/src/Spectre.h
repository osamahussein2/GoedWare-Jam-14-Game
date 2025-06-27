#ifndef SPECTRES_H
#define SPECTRES_H

#include "Character.h"

enum class SpectreState
{
	SLEEPING,
	WAKING_UP,
	MOVING
};

class Spectre : public Character
{
public:
	Spectre();
	~Spectre();

	virtual void InitializeCharacter() override {}
	virtual void InitializeCharacter(std::string childNode2_) override;
	virtual void DrawCharacter() override;
	virtual void UnloadCharacter() override;

	virtual void ResetCharacter() {}
	virtual void ResetCharacter(std::string childNode2_);

private:
	float updateTime;

	float failedTimer;

	static constexpr float distanceThreshold = 75.0f;

	SpectreState spectreState;

	Vector2 lastPosition;

	// Private functions
	void ChangeUpdateAnimationTime();
};

#endif