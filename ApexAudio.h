#pragma once

#include <SFML/Audio.hpp>

class ApexAudio
{
public:
	enum class Sound
	{
		GUN_FIRE, GUN_FIRE_EMPTY, GUN_RELOAD,
		BOOP,
		// All values must be above this line
		_LAST_ELEMENT
	};
	enum class Music
	{
		BG_SONG, OVERWORLD_BGM,

		// All values must be above this line
		_LAST_ELEMENT
	};

	static bool LoadSounds();
	
	static void PlaySoundEffect(Sound sound);
	static void PlayMusicTrack(Music track);
	static void SetAllPaused(bool paused);

	static void SetSoundPitch(Sound sound, float pitch);
	static void ResetSoundPitch(Sound sound);
	static void SlideSoundPitch(Sound sound, float deltaPitch);
	static void SetMusicPitch(Music track, float pitch);
	static void ResetMusicPitch(Music track);
	static void SlideMusicPitch(Music track, float deltaPitch);

	static void SetAllSoundsVolume(float volume);
	static void SetSoundVolume(Sound sound, float volume);
	static void SetAllMusicVolume(float volume);
	static void SetMusicVolume(Music track, float volume);
	

private:
	ApexAudio() = delete;

	static void LoadSound(Sound sound, const std::string& filePath);
	static void LoadMusicTrack(Music track, const std::string& filePath, bool loop);

	static bool m_IsInitialized;

	struct ApexSoundEffect
	{
		sf::SoundBuffer m_Buffer;
		sf::Sound m_Sound;
	};

	static ApexSoundEffect m_SoundEffects[int(Sound::_LAST_ELEMENT)];
	static sf::Music m_MusicTracks[int(Music::_LAST_ELEMENT)];

};
