#pragma once

#include <APEX\Graphics\TextureManager.h>

#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Image.hpp>
#include <SFML\Graphics\Sprite.hpp>
#include <SFML\System\Time.hpp>

namespace apex
{
	class SpriteSheet
	{
	public:
		struct Sequence
		{
			sf::Vector2i startFrameIndex = sf::Vector2i();
			sf::Int32 msPerFrame = 0;
			sf::Uint32 framesLong = 1;
			sf::Uint8 currentFrame = 0;
			sf::Time timeElapsedThisFrame = sf::Time::Zero;
			bool pingPongs = false; // If this is true the animation goes: [0, 1, 2, 3, 2, 1]
			bool movingForward = true; // Only used if ping ponging
		};

		SpriteSheet();
		SpriteSheet(sf::Texture* texture, sf::Uint8 frameWidth, sf::Uint8 frameHeight);
		virtual ~SpriteSheet();

		void Create(sf::Texture* texture, sf::Uint8 frameWidth, sf::Uint8 frameHeight);

		void Tick(sf::Time elapsed);
		void Draw(sf::RenderTarget& target, sf::RenderStates states, sf::Uint8 col, sf::Uint8 row); // Draws specific frame
		void Draw(sf::RenderTarget& target, sf::RenderStates states); // Draws the current frame of the current sequence

		sf::Uint8 GetCurrentFrame() const;
		float GetFrameWidth() const;
		float GetFrameHeight() const;

		void AddSequence(sf::Uint8 sequenceIndex, Sequence sequence);
		sf::Uint8 GetCurrentSequenceIndex() const;
		void SetCurrentSequence(sf::Uint8 sequenceIndex, bool restartAnimation = true);

		bool HasRestarted() const;

		// Call this if the entire image is only one sequence
		void SetEntireSpriteAsOneSequence(sf::Int32 msPerFrame);
		void SetFrameIndex(sf::Uint8 frameIndex);

		sf::Uint8 GetCurrentSequenceFramesLong() const;

		void SetSpriteScale(const sf::Vector2f& scale);
		void SetSpriteScale(float scaleX, float scaleY);
		void ResetSpriteScale();

	private:
		sf::Sprite m_Sprite;
		sf::Uint8 m_FramesWide;
		sf::Uint8 m_FramesHigh;
		sf::Uint8 m_FrameWidth;
		sf::Uint8 m_FrameHeight;

		bool m_HasRestarted = false;

		std::vector<Sequence> m_Sequences;
		sf::Uint8 m_CurrentSequenceIndex;
	};
} // namespace apex
