#pragma once

#include "fmod.hpp"
#include <string>
#include <unordered_map>

class AudioEngine
{
public:
	static int ErrorCheck(FMOD_RESULT result);
	void init();
	void update();
	void shutdown();

	void loadSound(const std::string& soundName, const std::string& filename, bool b3d, bool bLooping = false, bool bStream = false);
	void unloadSound(const std::string& soundName);
	void playSoundByName(const std::string& soundName);

private:
	FMOD::System* pSystem;
	std::unordered_map<std::string, FMOD::Sound*> sounds;
};

