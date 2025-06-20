#ifndef PLAYER_H
#define PLAYER_H

#include "Character.h"

class Player : public Character
{
public:
	Player();
	~Player();

	static std::shared_ptr<Player> Instance();

	virtual void InitializeCharacter() override;
	void BeginFollowPlayerCamera();
	virtual void DrawCharacter() override;
	virtual void UnloadCharacter() override;

private:
	Camera2D camera{};

	const float updateTime{ 1.0f / 10.0f };

	static std::shared_ptr<Player> playerInstance;
};

#endif