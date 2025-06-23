#ifndef SPECTRES_H
#define SPECTRES_H

#include "Character.h"

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
	const float updateTime{ 1.0f / 12.0f };

	float failedTimer;

	static constexpr float distanceThreshold = 75.0f;
};

#endif