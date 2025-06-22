#ifndef AUDIO_H
#define AUDIO_H

#include <raylib.h>
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

class MusicAudio
{
public:
	MusicAudio();
	~MusicAudio();

	void InitializeMusic(std::string filePathNode_);
	void SetMusicAudioVolume(float volume_);
	void PlayMusic(bool looping_);
	void UnloadMusic();

private:
	Music music;

	rapidxml::file<> audioFile;
	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* rootNode;

	float timePlayed;

	bool isPlaying;
};

class SoundAudio
{
public:
	SoundAudio();
	~SoundAudio();

	void InitializeSound(std::string filePathNode_);
	void PlaySoundAudio();
	void UnloadSoundAudio();

private:
	Sound sound;

	rapidxml::file<> audioFile;
	rapidxml::xml_document<> document;
	rapidxml::xml_node<>* rootNode;
};

#endif