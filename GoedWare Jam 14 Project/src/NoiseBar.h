#ifndef NOISEBAR_H
#define NOISEBAR_H

#include "MyRectangle.h"

class NoiseBar : public MyRectangle
{
public:
	NoiseBar();
	~NoiseBar();

	virtual void InitializeSprite(std::string childNode2_) override;
	void InitializeCurrentBar(std::string tag_);
	virtual void DrawSprite(Color color_) override;

	void UpdateCurrentNoise(std::string tag_);

	bool isNoiseIncreased;

	float GetCurrentNoise() const { return currentNoise; }
	float GetMaxNoiseThreshold() const { return maxNoiseThreshold; }
	bool GetNoiseMaxedOut() const { return noiseMaxedOut; }

	void SetCurrentNoiseToMaximum() { currentNoise = maxNoiseThreshold; }

	void ResetCurrentNoiseValue();

	float GetCurrentNoiseVelocity() const { return currentNoiseVelocity; }

	void SetCurrentNoiseVelocity(float currentNoiseVelocity_) 
	{ 
		if (currentNoiseVelocity != currentNoiseVelocity_) currentNoiseVelocity = currentNoiseVelocity_;
	}

private:
	float currentNoise;
	float maxNoiseThreshold;

	bool noiseMaxedOut;

	float currentNoiseVelocity;
};

#endif