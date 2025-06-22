#ifndef WORLD_H
#define WORLD_H

#include "GameObject.h"

class World : public GameObject
{
public:
	World();
	~World();

	static std::shared_ptr<World> Instance();

	void InitializeWorld(const char* childNode2_);
	void DrawWorld(bool lightOn_, Color color_);
	void UnloadWorld();

private:
	static std::shared_ptr<World> worldInstance;
};

#endif