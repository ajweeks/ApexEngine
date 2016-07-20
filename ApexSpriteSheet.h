#pragma once

#include "TextureManager.h"

#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Image.hpp>
#include <SFML\Graphics\Sprite.hpp>
#include <SFML\System\Time.hpp>

class ApexSpriteSheet
{
public:
	struct Sequence
	{
		sf::Vector2i startFrameIndex = sf::Vector2i();
		sf::Int32 msPerFrame = 0;
		sf::Uint32 framesLong = 1;
		sf::Uint8 currentFrame = 0;
		sf::Time timeElapsedThisFrame = sf::Time::Zero;
	};

	ApexSpriteSheet();
	ApexSpriteSheet(sf::Texture* texture, sf::Uint8 frameWidth, sf::Uint8 frameHeight);
	virtual ~ApexSpriteSheet();

	void Create(sf::Texture* texture, sf::Uint8 frameWidth, sf::Uint8 frameHeight);

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states, sf::Uint8 col, sf::Uint8 row); // Draws specific frame
	void Draw(sf::RenderTarget& target, sf::RenderStates states); // Draws the current frame of the current sequence

	sf::Uint8 GetCurrentFrame() const;

	void AddSequence(sf::Uint8 sequenceIndex, Sequence sequence);
	void SetCurrentSequence(sf::Uint8 sequenceIndex, bool restartAnimation = true);
	
	bool HasRestarted() const;

	// Call this if the entire image is only one sequence
	void SetEntireSpriteAsOneSequence(sf::Int32 msPerFrame);

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
