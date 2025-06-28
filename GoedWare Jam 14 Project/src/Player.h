#ifndef PLAYER_H
#define PLAYER_H

#include "Character.h"
#include "NoiseBar.h"
#include "Audio.h"
#include "Timer.h"
#include <unordered_map>
#include "GameObject.h"
#include "Puddle.h"
#include "Branch.h"
#include <array>

constexpr int FOOTSTEPS_SIZE = 8;

class Player : public Character
{
public:
	Player();
	~Player();

	static std::shared_ptr<Player> Instance();

	virtual void InitializeCharacter() override;
	virtual void InitializeCharacter(std::string childNode2_) override {}
	void BeginFollowPlayerCamera();
	virtual void DrawCharacter() override;
	virtual void UnloadCharacter() override;

	void DrawGameObjects();

	virtual void ResetCharacter() override;
	virtual void ResetCharacter(std::string childNode2_) {}

	void DrawUI();

	bool GetLightOn() const { return lightOn; }
	Vector2 GetCenter() const { return center; }
	float GetRadius() const { return circleRadius; }
	
	Vector2 GetPosition() const { return position; }
	Rectangle GetRectangle() const { return rectangle; }

	int GetNoiseValue();
	int GetMaxNoiseThresholdValue();

	bool GetCurrentNoiseMaxedOut();

	void ReturnMaximumNoise();

	bool GetPlayerInputEnabled() const { return inputEnabled; }
	void SetPlayerInputEnabled(bool inputEnabled_);

	bool HasFailed() const;
	void SetHasFailed(bool hasFailed_);

	int GetLevelNumber() const { return levelNumber; }

	void SetPlayerNoiseVelocity(float noiseVelocity_);

	void CheckForLevel1BushCollision(Vector2& position_, Vector2& lastPosition_, Rectangle& rectangle_);
	void CheckForLevel2BushCollision(Vector2& position_, Vector2& lastPosition_, Rectangle& rectangle_);

private:
	Camera2D camera{};

	Timer timer;

	bool inputEnabled;

	bool hasFailed;

	const float updateTime{ 1.0f / 10.0f };

	static std::shared_ptr<Player> playerInstance;

	Vector2 center;
	const float circleRadius = 250.0f;

	bool lightOn;

	std::unordered_map<std::string, NoiseBar> noiseBars;

	int footstepsIndex;
	bool footstepsIndexSet;

	SoundAudio groundFootsteps[FOOTSTEPS_SIZE];

	std::unordered_map<std::string, GameObject> level1Bushes, level2Bushes;
	std::unordered_map<std::string, GameObject> level2Void;

	Vector2 lastPosition;

	int levelNumber;

	std::array<Puddle, 4> level1Puddles;
	std::array<Branch, 4> level1Branches;

	std::array<Puddle, 5> level2Puddles;
	std::array<Branch, 3> level2Branches;

	GameObject stair;

	// Private functions
	void CheckForLevel1BushCollision();
	void CheckForLevel2BushCollision();
};

#endif