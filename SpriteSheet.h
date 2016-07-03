#pragma once

#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Image.hpp>
#include <SFML\Graphics\RenderStates.hpp>
#include <SFML\Graphics\Sprite.hpp>
#include <SFML\System\Time.hpp>

class SpriteSheet
{
public:
	struct Sequence
	{
		sf::Vector2i startFrameIndex;
		sf::Int32 msPerFrame;
		int framesLong;
		int currentFrame;
		sf::Time timeElapsedThisFrame;
	};

	SpriteSheet(std::string filePath, int frameWidth, int frameHeight);
	virtual ~SpriteSheet();

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states, float left, float top, int col, int row); // Draws specific frame
	void Draw(sf::RenderTarget& target, sf::RenderStates states, float left, float top); // Draws the current frame of the current sequence

	void AddSequence(unsigned int sequenceIndex, Sequence sequence);
	void SetCurrentSequence(int sequenceIndex, bool restartAnimation = true);

private:
	sf::Texture m_Texture;
	sf::Sprite m_Sprite;
	int m_FramesWide;
	int m_FramesHigh;
	int m_FrameWidth;
	int m_FrameHeight;

	std::vector<Sequence> m_Sequences;
	unsigned int m_CurrentSequenceIndex;

};
