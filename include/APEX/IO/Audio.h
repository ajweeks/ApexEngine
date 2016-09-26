#pragma once

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <memory>

namespace apex
{
	class Audio
	{
	public:
		static void AddSoundEffect(size_t index, const std::string& filePath);
		static void AddSong(size_t index, const std::string& filePath, bool loop);

		static bool IsSoundEffectPlaying(size_t index);

		static void PlaySoundEffect(size_t index);
		static void PlayMusicTrack(size_t index);
		static void SetAllPaused(bool paused);

		static void SetSoundPitch(size_t index, float pitch);
		static void ResetSoundPitch(size_t index);
		static void SlideSoundPitch(size_t index, float deltaPitch);
		static void SetMusicPitch(size_t index, float pitch);
		static void ResetMusicPitch(size_t index);
		static void SlideMusicPitch(size_t index, float deltaPitch);

		static void SetAllSoundsVolume(float volume);
		static void SetSoundVolume(size_t index, float volume);
		static void SetAllMusicVolume(float volume);
		static void SetMusicVolume(size_t index, float volume);

	private:
		Audio() = delete;

		struct SoundEffect
		{
			sf::SoundBuffer m_Buffer;
			sf::Sound m_Sound;
		};

		static std::vector<SoundEffect> s_SoundEffects;
		static std::vector<std::unique_ptr<sf::Music>> s_MusicTracks;

	};
} // namespace apex
