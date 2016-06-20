
#include "ApexAudio.h"

ApexAudio::ApexSoundEffect ApexAudio::m_SoundEffects[];
sf::Music ApexAudio::m_MusicTracks[];
bool ApexAudio::m_IsInitialized = false;

bool ApexAudio::LoadSounds()
{
	// SOUNDS
	if (!m_SoundEffects[int(Sound::WALK_WOOD)].m_Buffer.loadFromFile("resources/sound/wood_stair_step_01.wav")) return false;
	m_SoundEffects[int(Sound::WALK_WOOD)].m_Sound.setBuffer(m_SoundEffects[int(Sound::WALK_WOOD)].m_Buffer);

	if (!m_SoundEffects[int(Sound::COIN)].m_Buffer.loadFromFile("resources/sound/coin-collect.wav")) return false;
	m_SoundEffects[int(Sound::COIN)].m_Sound.setBuffer(m_SoundEffects[int(Sound::COIN)].m_Buffer);

	// SONGS
	if (!m_MusicTracks[int(Music::BG_SONG)].openFromFile("resources/sound/music/map-1-yoshis-island.wav")) return false;
	m_MusicTracks[int(Music::BG_SONG)].setLoop(true);
	if (!m_MusicTracks[int(Music::OVERWORLD_BGM)].openFromFile("resources/sound/music/overworld-bgm.wav")) return false;
	m_MusicTracks[int(Music::OVERWORLD_BGM)].setLoop(true);

	m_IsInitialized = true;
	return true;
}

void ApexAudio::PlaySoundEffect(Sound sound)
{
	m_SoundEffects[int(sound)].m_Sound.play();
}

void ApexAudio::PlayMusicTrack(Music track)
{
	m_MusicTracks[int(track)].play();
}

void ApexAudio::SetAllPaused(bool paused)
{
	if (paused)
	{
		for (size_t i = 0; i < int(Sound::_LAST_ELEMENT); ++i)
		{
			m_SoundEffects[i].m_Sound.pause();
		}
		for (size_t i = 0; i < int(Music::_LAST_ELEMENT); ++i)
		{
			m_MusicTracks[i].pause();
		}
	}
	else
	{
		for (size_t i = 0; i < int(Sound::_LAST_ELEMENT); ++i)
		{
			if (m_SoundEffects[i].m_Sound.getStatus() == sf::Sound::Paused) m_SoundEffects[i].m_Sound.play();
		}
		for (size_t i = 0; i < int(Music::_LAST_ELEMENT); ++i)
		{
			if (m_MusicTracks[i].getStatus() == sf::Sound::Paused) m_MusicTracks[i].play();
		}
	}
}

void ApexAudio::SetSoundPitch(Sound sound, float pitch)
{
	m_SoundEffects[int(sound)].m_Sound.setPitch(pitch);
}

void ApexAudio::ResetSoundPitch(Sound sound)
{
	m_SoundEffects[int(sound)].m_Sound.setPitch(1.0f);
}

void ApexAudio::SlideSoundPitch(Sound sound, float deltaPitch)
{
	m_SoundEffects[int(sound)].m_Sound.setPitch(m_SoundEffects[int(sound)].m_Sound.getPitch() + deltaPitch);
}

void ApexAudio::SetMusicPitch(Music track, float pitch)
{
	m_MusicTracks[int(track)].setPitch(pitch);
}

void ApexAudio::ResetMusicPitch(Music track)
{
	m_MusicTracks[int(track)].setPitch(1.0f);
}

void ApexAudio::SlideMusicPitch(Music track, float deltaPitch)
{
	m_MusicTracks[int(track)].setPitch(m_MusicTracks[int(track)].getPitch() + deltaPitch);
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
	m_SoundEffects[int(sound)].m_Sound.setVolume(volume);
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
	m_MusicTracks[int(track)].setVolume(volume);
}
