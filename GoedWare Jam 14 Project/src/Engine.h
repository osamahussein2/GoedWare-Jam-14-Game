#ifndef ENGINE_H
#define ENGINE_H

#include <memory>

class Engine
{
public:
	Engine();
	~Engine();

	static std::shared_ptr<Engine> Instance();

	void RunEngine();

private:
	static std::shared_ptr<Engine> engineInstance;
};

#endif