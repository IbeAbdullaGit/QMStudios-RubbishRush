#pragma once

#include "fmod.hpp"
#include "fmod_studio.hpp"
#include "fmod_common.h"
#include <string>
#include <unordered_map>
#include "ToneFire.h"

class AudioEngine
{
public:
	static int ErrorCheck(FMOD_RESULT result);
	static void init();
	static void studioinit();
	void update();
	static void studioupdate();
	void shutdown();
	void studioshutdown();

	void loadSound(const std::string& soundName, const std::string& filename, bool b3d, bool bLooping = false, bool bStream = false);
	static void loadBank(const std::string& filename);
	void unloadSound(const std::string& soundName);
	static void loadEventS(const std::string& eventname);
	static void playEventS(const std::string& eventname);
	void playSoundByName(const std::string& soundName);
	static ToneFire::FMODStudio studio;
	static ToneFire::StudioSound audio;
private:
	FMOD::System* pSystem;
	
	std::unordered_map<std::string, FMOD::Sound*> sounds;
};

