#ifndef PLAYER_H
#define PLAYER_H

#include "Character.h"
#include "NoiseBar.h"
#include <unordered_map>

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

	void DrawUI();

	bool GetLightOn() { return lightOn; }
	Vector2 GetCenter() { return center; }
	float GetRadius() const { return circleRadius; }

private:
	Camera2D camera{};

	const float updateTime{ 1.0f / 10.0f };

	static std::shared_ptr<Player> playerInstance;

	Vector2 center;
	const float circleRadius = 250.0f;

	bool lightOn;

	std::unordered_map<std::string, NoiseBar> noiseBars;
};

#endif