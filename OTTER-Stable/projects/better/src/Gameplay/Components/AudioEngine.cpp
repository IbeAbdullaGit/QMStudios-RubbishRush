#include "AudioEngine.h"
#include "fmod_errors.h"
#include "fmod_studio_common.h"
#include "fmod_studio.hpp"
#include <iostream>
int AudioEngine::ErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK) 
	{
		std::cout << "FMOD ERROR:" << FMOD_ErrorString(result) << std::endl;

#ifdef _DEBUG
		__debugbreak();
#endif // _DEBUG
		return 1;
	}
	return 0;
}

void AudioEngine::init()
{
	ErrorCheck(FMOD::System_Create(&pSystem));
	ErrorCheck(pSystem->init(32, FMOD_INIT_NORMAL, nullptr));
}

void AudioEngine::studioinit()
{
	ErrorCheck(FMOD::Studio::System::create(&pStudioSystem));
	ErrorCheck(pStudioSystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0));
}

void AudioEngine::update()
{
	ErrorCheck(pSystem->update());
}

void AudioEngine::studioupdate()
{
	ErrorCheck(pStudioSystem->update());
}

void AudioEngine::shutdown()
{
	ErrorCheck(pSystem->close());
	ErrorCheck(pSystem->release());
}

void AudioEngine::studioshutdown()
{
ErrorCheck(pStudioSystem->release());
}

void AudioEngine::loadSound(const std::string& soundName, const std::string& filename, bool b3d, bool bLooping, bool bStream)
{
	//checks if already loaded
	auto foundElement = sounds.find(soundName);
	if (foundElement != sounds.end())
	{
		return;
	}

	FMOD_MODE mode = FMOD_DEFAULT;
	mode |= (b3d) ? FMOD_3D : FMOD_2D;
	mode |= (bLooping) ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	mode |= (bStream) ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD::Sound* LoadedSound;
	ErrorCheck(pSystem->createSound(filename.c_str(), mode, nullptr, &LoadedSound));
	
	if (LoadedSound != nullptr)
	{
		sounds[soundName] = LoadedSound;
	}
}

void AudioEngine::loadBank(const char* filename, FMOD::Studio::Bank** bank)
{
	ErrorCheck(pStudioSystem->loadBankFile(filename, FMOD_STUDIO_LOAD_BANK_NORMAL, bank));
}

//FMOD_RESULT AudioEngine::s(const char* filename, FMOD::Studio::Bank** bank = NULL)
//{
//	return pStudioSystem->loadBankFile(filename, FMOD_STUDIO_LOAD_BANK_NORMAL, bank);
//}



void AudioEngine::unloadSound(const std::string& soundName)
{
	auto foundElement = sounds.find(soundName);
	if (foundElement != sounds.end())
	{
		ErrorCheck(foundElement->second->release());
		sounds.erase(foundElement);
	}
}

void AudioEngine::getEventS(const char* pathname,FMOD::Studio::EventDescription* eventd, FMOD::Studio::EventInstance** eventInst)
{
	ErrorCheck(pStudioSystem->getEvent(pathname, &eventd));


	ErrorCheck(eventd->createInstance(eventInst));
}

void AudioEngine::playEvent(FMOD::Studio::EventInstance* eventInst)
{
	ErrorCheck(eventInst->start());
}





void AudioEngine::playSoundByName(const std::string& soundName)
{
	pSystem->playSound(sounds[soundName], nullptr, false, nullptr);
}
