#pragma once

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <array>

namespace apex
{
	class Audio
	{
	public:
		enum class Sound
		{
			GUN_FIRE, GUN_FIRE_EMPTY, GUN_RELOAD,
			BOOP, BLIP,
			TYPING_1, TYPING_2, TYPING_3,
			COIN_PICKUP,

			_LAST_ELEMENT
		};
		enum class Music
		{
			A, // There needs to be at least one element in here to compile
			_LAST_ELEMENT
		};

		static bool LoadSounds();

		static bool IsSoundEffectPlaying(Sound sound);

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
		Audio() = delete;

		static void LoadSound(Sound sound, const std::string& filePath);
		static void LoadMusicTrack(Music track, const std::string& filePath, bool loop);

		static bool s_IsInitialized;

		struct SoundEffect
		{
			sf::SoundBuffer m_Buffer;
			sf::Sound m_Sound;
		};

		static std::array<SoundEffect, int(Sound::_LAST_ELEMENT)> s_SoundEffects;
		static std::array<sf::Music, int(Music::_LAST_ELEMENT)> s_MusicTracks;

	};
} // namespace apex
