#include "Audio.h"

/*

    MUSIC AUDIO CODE

*/

MusicAudio::MusicAudio() : music(), audioFile("XML files/Audio.xml"), rootNode(), isPlaying(false), timePlayed(0.0f)
{
}

MusicAudio::~MusicAudio()
{
}

void MusicAudio::InitializeMusic(std::string filePathNode_)
{
    document.parse<0>(audioFile.data());

    rootNode = document.first_node("Audio");

    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("AudioInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node(filePathNode_.c_str()); fileNode2;
            fileNode2 = fileNode->next_sibling())
        {
            music = LoadMusicStream(fileNode2->first_attribute("audioFilePath")->value());
        }
    }
}

void MusicAudio::SetMusicAudioVolume(float volume_)
{
    SetMusicVolume(music, volume_);
}

void MusicAudio::PlayMusic(bool looping_)
{
    UpdateMusicStream(music);

    if (looping_ == true)
    {
        // Loop music if it's not playing
        if (!IsMusicStreamPlaying(music)) PlayMusicStream(music);
    }

    else if (looping_ == false)
    {
        // Otherwise, just play music without looping it
        PlayMusicStream(music);
    }
}

void MusicAudio::UnloadMusic()
{
    UnloadMusicStream(music);
}

/*

    SOUND AUDIO CODE

*/

SoundAudio::SoundAudio() : sound(), audioFile("XML files/Audio.xml"), rootNode()
{
}

SoundAudio::~SoundAudio()
{
}

void SoundAudio::InitializeSound(std::string filePathNode_)
{
    document.parse<0>(audioFile.data());

    rootNode = document.first_node("Audio");

    for (rapidxml::xml_node<>* fileNode = rootNode->first_node("AudioInfo"); fileNode;
        fileNode = fileNode->next_sibling())
    {
        for (rapidxml::xml_node<>* fileNode2 = fileNode->first_node(filePathNode_.c_str()); fileNode2;
            fileNode2 = fileNode->next_sibling())
        {
            sound = LoadSound(fileNode2->first_attribute("audioFilePath")->value());
        }
    }
}

void SoundAudio::PlaySoundAudio()
{
    if (!IsSoundPlaying(sound)) PlaySound(sound);
}

void SoundAudio::UnloadSoundAudio()
{
    UnloadSound(sound);
}