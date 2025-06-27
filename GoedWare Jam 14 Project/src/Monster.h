#ifndef MONSTER_H
#define MONSTER_H

#include "Character.h"

enum class MonsterState
{
	SLEEPING,
	WAKING_UP,
	MOVING
};

class Monster : public Character
{
public:
	Monster();
	~Monster();

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

	MonsterState monsterState;

	Vector2 lastPosition;

	// Private functions
	void ChangeUpdateAnimationTime();
};

#endif