
#include <APEX\IO\Audio.h>
#include <APEX\ApexMain.h>

namespace apex
{
	std::array<Audio::SoundEffect, int(Audio::Sound::_LAST_ELEMENT)> Audio::s_SoundEffects;
	std::array<sf::Music, int(Audio::Music::_LAST_ELEMENT)> Audio::s_MusicTracks;
	bool Audio::s_IsInitialized = false;

	bool Audio::LoadSounds()
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

	void Audio::LoadSound(Sound sound, const std::string& filePath)
	{
		if (!s_SoundEffects[int(sound)].m_Buffer.loadFromFile(filePath))
		{
			apex::PrintString("Unable to load sound effect " + filePath + "\n");
			return;
		}
		s_SoundEffects[int(sound)].m_Sound.setBuffer(s_SoundEffects[int(sound)].m_Buffer);
	}

	void Audio::LoadMusicTrack(Music track, const std::string& filePath, bool loop)
	{
		if (!s_MusicTracks[int(track)].openFromFile(filePath))
		{
			apex::PrintString("Unable to music track " + filePath + "\n");
			return;
		}
		s_MusicTracks[int(track)].setLoop(loop);
	}

	bool Audio::IsSoundEffectPlaying(Sound sound)
	{
		return s_SoundEffects[int(sound)].m_Sound.getStatus() == sf::Sound::Playing;
	}

	void Audio::PlaySoundEffect(Sound sound)
	{
		s_SoundEffects[int(sound)].m_Sound.play();
	}

	void Audio::PlayMusicTrack(Music track)
	{
		s_MusicTracks[int(track)].play();
	}

	void Audio::SetAllPaused(bool paused)
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

	void Audio::SetSoundPitch(Sound sound, float pitch)
	{
		s_SoundEffects[int(sound)].m_Sound.setPitch(pitch);
	}

	void Audio::ResetSoundPitch(Sound sound)
	{
		s_SoundEffects[int(sound)].m_Sound.setPitch(1.0f);
	}

	void Audio::SlideSoundPitch(Sound sound, float deltaPitch)
	{
		s_SoundEffects[int(sound)].m_Sound.setPitch(s_SoundEffects[int(sound)].m_Sound.getPitch() + deltaPitch);
	}

	void Audio::SetMusicPitch(Music track, float pitch)
	{
		s_MusicTracks[int(track)].setPitch(pitch);
	}

	void Audio::ResetMusicPitch(Music track)
	{
		s_MusicTracks[int(track)].setPitch(1.0f);
	}

	void Audio::SlideMusicPitch(Music track, float deltaPitch)
	{
		s_MusicTracks[int(track)].setPitch(s_MusicTracks[int(track)].getPitch() + deltaPitch);
	}

	void Audio::SetAllSoundsVolume(float volume)
	{
		for (size_t i = 0; i < int(Sound::_LAST_ELEMENT); ++i)
		{
			SetSoundVolume(Sound(i), volume);
		}
	}

	void Audio::SetSoundVolume(Sound sound, float volume)
	{
		s_SoundEffects[int(sound)].m_Sound.setVolume(volume * 100.0f);
	}

	void Audio::SetAllMusicVolume(float volume)
	{
		for (size_t i = 0; i < int(Music::_LAST_ELEMENT); ++i)
		{
			SetMusicVolume(Music(i), volume);
		}
	}

	void Audio::SetMusicVolume(Music track, float volume)
	{
		s_MusicTracks[int(track)].setVolume(volume * 100.0f);
	}
} // namespace apex

