
#include "ApexAudio.h"
#include "ApexMain.h"

std::array<ApexAudio::ApexSoundEffect, int(ApexAudio::Sound::_LAST_ELEMENT)> ApexAudio::s_SoundEffects;
std::array<sf::Music, int(ApexAudio::Music::_LAST_ELEMENT)> ApexAudio::s_MusicTracks;
bool ApexAudio::s_IsInitialized = false;

bool ApexAudio::LoadSounds()
{
	// SOUNDS
	LoadSound(Sound::GUN_FIRE, "resources/sound/gun-fire.wav");
	LoadSound(Sound::GUN_FIRE_EMPTY, "resources/sound/gun-fire-empty.wav");
	LoadSound(Sound::GUN_RELOAD, "resources/sound/gun-reload.wav");
	LoadSound(Sound::BOOP, "resources/sound/boop.wav");
	LoadSound(Sound::BLIP, "resources/sound/blip.wav");
	LoadSound(Sound::COIN_PICKUP, "resources/sound/coin-pickup.wav");

	LoadSound(Sound::TYPING_1, "resources/sound/typing-1.wav");
	LoadSound(Sound::TYPING_2, "resources/sound/typing-2.wav");
	LoadSound(Sound::TYPING_3, "resources/sound/typing-3.wav");

	// SONGS

	s_IsInitialized = true;
	return true;
}

void ApexAudio::LoadSound(Sound sound, const std::string& filePath)
{
	if (!s_SoundEffects[int(sound)].m_Buffer.loadFromFile(filePath))
	{
		ApexOutputDebugString("Unable to load sound effect " + filePath + "\n");
		return;
	}
	s_SoundEffects[int(sound)].m_Sound.setBuffer(s_SoundEffects[int(sound)].m_Buffer);
}

void ApexAudio::LoadMusicTrack(Music track, const std::string& filePath, bool loop)
{
	if (!s_MusicTracks[int(track)].openFromFile(filePath))
	{
		ApexOutputDebugString("Unable to music track " + filePath + "\n");
		return;
	}
	s_MusicTracks[int(track)].setLoop(loop);
}

bool ApexAudio::IsSoundEffectPlaying(Sound sound)
{
	return s_SoundEffects[int(sound)].m_Sound.getStatus() == sf::Sound::Playing;
}

void ApexAudio::PlaySoundEffect(Sound sound)
{
	s_SoundEffects[int(sound)].m_Sound.play();
}

void ApexAudio::PlayMusicTrack(Music track)
{
	s_MusicTracks[int(track)].play();
}

void ApexAudio::SetAllPaused(bool paused)
{
	if (paused)
	{
		for (size_t i = 0; i < int(Sound::_LAST_ELEMENT); ++i)
		{
			s_SoundEffects[i].m_Sound.pause();
		}
		for (size_t i = 0; i < int(Music::_LAST_ELEMENT); ++i)
		{
			s_MusicTracks[i].pause();
		}
	}
	else
	{
		for (size_t i = 0; i < int(Sound::_LAST_ELEMENT); ++i)
		{
			if (s_SoundEffects[i].m_Sound.getStatus() == sf::Sound::Paused) s_SoundEffects[i].m_Sound.play();
		}
		for (size_t i = 0; i < int(Music::_LAST_ELEMENT); ++i)
		{
			if (s_MusicTracks[i].getStatus() == sf::Sound::Paused) s_MusicTracks[i].play();
		}
	}
}

void ApexAudio::SetSoundPitch(Sound sound, float pitch)
{
	s_SoundEffects[int(sound)].m_Sound.setPitch(pitch);
}

void ApexAudio::ResetSoundPitch(Sound sound)
{
	s_SoundEffects[int(sound)].m_Sound.setPitch(1.0f);
}

void ApexAudio::SlideSoundPitch(Sound sound, float deltaPitch)
{
	s_SoundEffects[int(sound)].m_Sound.setPitch(s_SoundEffects[int(sound)].m_Sound.getPitch() + deltaPitch);
}

void ApexAudio::SetMusicPitch(Music track, float pitch)
{
	s_MusicTracks[int(track)].setPitch(pitch);
}

void ApexAudio::ResetMusicPitch(Music track)
{
	s_MusicTracks[int(track)].setPitch(1.0f);
}

void ApexAudio::SlideMusicPitch(Music track, float deltaPitch)
{
	s_MusicTracks[int(track)].setPitch(s_MusicTracks[int(track)].getPitch() + deltaPitch);
}

void ApexAudio::SetAllSoundsVolume(float volume)
{
	for (size_t i = 0; i < int(Sound::_LAST_ELEMENT); ++i)
	{
		SetSoundVolume(Sound(i), volume);
	}
}

void ApexAudio::SetSoundVolume(Sound sound, float volume)
{
	s_SoundEffects[int(sound)].m_Sound.setVolume(volume * 100.0f);
}

void ApexAudio::SetAllMusicVolume(float volume)
{
	for (size_t i = 0; i < int(Music::_LAST_ELEMENT); ++i)
	{
		SetMusicVolume(Music(i), volume);
	}
}

void ApexAudio::SetMusicVolume(Music track, float volume)
{
	s_MusicTracks[int(track)].setVolume(volume * 100.0f);
}

