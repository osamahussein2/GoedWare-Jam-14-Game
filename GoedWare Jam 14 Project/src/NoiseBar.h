#ifndef NOISEBAR_H
#define NOISEBAR_H

#include "MyRectangle.h"

class NoiseBar : public MyRectangle
{
public:
	NoiseBar();
	~NoiseBar();

	virtual void InitializeSprite(const char* childNode2_) override;
	void InitializeCurrentBar(std::string tag_);
	virtual void DrawSprite(Color color_) override;

	void UpdateCurrentNoise(std::string tag_);

	bool isNoiseIncreased;

private:
	float currentNoise;
	float maxNoiseThreshold;
};

#endif