
#include "Apex\IO\Audio.h"
#include "Apex\ApexMain.h"

namespace apex
{
	std::vector<Audio::SoundEffect> Audio::s_SoundEffects;
	std::vector<std::unique_ptr<sf::Music>> Audio::s_MusicTracks;

	void Audio::AddSoundEffect(size_t index, const std::string& filePath)
	{
		if(index >= s_SoundEffects.size()) s_SoundEffects.resize(index + 1);

		if (!s_SoundEffects[index].m_Buffer.loadFromFile(filePath))
		{
			PrintString("Unable to load sound effect " + filePath + "\n", LogType::LOG_ERROR);
			return;
		}
		s_SoundEffects[index].m_Sound.setBuffer(s_SoundEffects[index].m_Buffer);
	}

	void Audio::AddSong(size_t index, const std::string& filePath, bool loop)
	{
		if (index >= s_MusicTracks.size()) s_MusicTracks.resize(index + 1);

		if (!s_MusicTracks[index]->openFromFile(filePath))
		{
			PrintString("Unable to music track " + filePath + "\n", LogType::LOG_ERROR);
			return;
		}
		s_MusicTracks[index]->setLoop(loop);
	}

	bool Audio::IsSoundEffectPlaying(size_t index)
	{
		return s_SoundEffects[index].m_Sound.getStatus() == sf::Sound::Playing;
	}

	void Audio::PlaySoundEffect(size_t index)
	{
		s_SoundEffects[index].m_Sound.play();
	}

	void Audio::PlayMusicTrack(size_t index)
	{
		s_MusicTracks[index]->play();
	}

	void Audio::SetAllPaused(bool paused)
	{
		if (paused)
		{
			for (size_t i = 0; i < s_SoundEffects.size(); ++i)
			{
				s_SoundEffects[i].m_Sound.pause();
			}
			for (size_t i = 0; i < s_MusicTracks.size(); ++i)
			{
				s_MusicTracks[i]->pause();
			}
		}
		else
		{
			for (size_t i = 0; i < s_SoundEffects.size(); ++i)
			{
				if (s_SoundEffects[i].m_Sound.getStatus() == sf::Sound::Paused) s_SoundEffects[i].m_Sound.play();
			}
			for (size_t i = 0; i < s_MusicTracks.size(); ++i)
			{
				if (s_MusicTracks[i]->getStatus() == sf::Sound::Paused) s_MusicTracks[i]->play();
			}
		}
	}

	void Audio::SetSoundPitch(size_t index, float pitch)
	{
		s_SoundEffects[int(index)].m_Sound.setPitch(pitch);
	}

	void Audio::ResetSoundPitch(size_t index)
	{
		s_SoundEffects[int(index)].m_Sound.setPitch(1.0f);
	}

	void Audio::SlideSoundPitch(size_t index, float deltaPitch)
	{
		s_SoundEffects[int(index)].m_Sound.setPitch(s_SoundEffects[int(index)].m_Sound.getPitch() + deltaPitch);
	}

	void Audio::SetMusicPitch(size_t index, float pitch)
	{
		s_MusicTracks[int(index)]->setPitch(pitch);
	}

	void Audio::ResetMusicPitch(size_t index)
	{
		s_MusicTracks[int(index)]->setPitch(1.0f);
	}

	void Audio::SlideMusicPitch(size_t index, float deltaPitch)
	{
		s_MusicTracks[int(index)]->setPitch(s_MusicTracks[int(index)]->getPitch() + deltaPitch);
	}

	void Audio::SetAllSoundsVolume(float volume)
	{
		for (size_t i = 0; i < s_SoundEffects.size(); ++i)
		{
			SetSoundVolume(i, volume);
		}
	}

	void Audio::SetSoundVolume(size_t index, float volume)
	{
		s_SoundEffects[index].m_Sound.setVolume(volume * 100.0f);
	}

	void Audio::SetAllMusicVolume(float volume)
	{
		for (size_t i = 0; i < s_MusicTracks.size(); ++i)
		{
			SetMusicVolume(i, volume);
		}
	}

	void Audio::SetMusicVolume(size_t index, float volume)
	{
		s_MusicTracks[index]->setVolume(volume * 100.0f);
	}
} // namespace apex

