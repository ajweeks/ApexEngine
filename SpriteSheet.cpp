
#include "SpriteSheet.h"
#include <SFML\Graphics\RenderTarget.hpp>
#include <iostream>
#include <assert.h>

SpriteSheet::SpriteSheet(std::string filePath, int frameWidth, int frameHeight) :
	m_FrameWidth(frameWidth), m_FrameHeight(frameHeight)
{
	assert(filePath != "" && frameWidth != 0 && frameHeight != 0);

	if (!m_Texture.loadFromFile(filePath))
	{
		throw std::invalid_argument("Could not find file: \"" + filePath + "\"");
		return;
	}
	m_Sprite.setTexture(m_Texture);

	m_FramesWide = m_Texture.getSize().x / m_FrameWidth;
	m_FramesHigh = m_Texture.getSize().y / m_FrameHeight;
}

SpriteSheet::~SpriteSheet()
{
}

void SpriteSheet::Tick(sf::Time elapsed)
{
	if (m_CurrentSequenceIndex < m_Sequences.size())
	{
		Sequence& currentSequence = m_Sequences[m_CurrentSequenceIndex];
		currentSequence.timeElapsedThisFrame += elapsed;
		if (currentSequence.timeElapsedThisFrame.asMilliseconds() >= currentSequence.msPerFrame)
		{
			currentSequence.timeElapsedThisFrame = sf::milliseconds(
				currentSequence.timeElapsedThisFrame.asMilliseconds() - currentSequence.msPerFrame);
			if (++currentSequence.currentFrame == currentSequence.framesLong) currentSequence.currentFrame = 0;
		}
	}
}

void SpriteSheet::Draw(sf::RenderTarget& target, sf::RenderStates states, float left, float top, int col, int row)
{
	const sf::IntRect srcRect(col * m_FrameWidth, row * m_FrameHeight, m_FrameWidth, m_FrameHeight);
	m_Sprite.setTextureRect(srcRect);

	states.transform.translate(left, top);
	target.draw(m_Sprite, states);
}

void SpriteSheet::Draw(sf::RenderTarget& target, sf::RenderStates states, float left, float top)
{
	if (m_CurrentSequenceIndex >= m_Sequences.size())
	{
		throw std::logic_error("Attempted to draw unknown sequence: " + m_CurrentSequenceIndex);
		return;
	}

	Sequence& currentSequence = m_Sequences[m_CurrentSequenceIndex];
	int col = currentSequence.startFrameIndex.x + currentSequence.currentFrame;
	int row = currentSequence.startFrameIndex.y;
	while (col > m_FramesWide) 
	{
		col -= m_FramesWide;
		++row;
	}

	const sf::IntRect srcRect(col * m_FrameWidth, row * m_FrameHeight, m_FrameWidth, m_FrameHeight);
	m_Sprite.setTextureRect(srcRect);

	states.transform.translate(left - m_FrameWidth / 2, top - m_FrameHeight / 2);
	target.draw(m_Sprite, states);
}

void SpriteSheet::AddSequence(unsigned int sequenceIndex, Sequence sequence)
{
	if (sequenceIndex == m_Sequences.size()) 
	{
		m_Sequences.push_back(sequence);
	}
	else
	{
		if (sequenceIndex > m_Sequences.size()) // Allow out of order additions (index 3, followed by index 1 for example)
		{
			m_Sequences.resize(sequenceIndex);
		}
		m_Sequences[sequenceIndex] = sequence;
	}
}

void SpriteSheet::SetCurrentSequence(int sequenceIndex, bool restartAnimation)
{
	if (m_CurrentSequenceIndex >= m_Sequences.size())
	{
		throw std::logic_error("Attempted to set unknown sequence index: " + sequenceIndex);
		return;
	}

	m_CurrentSequenceIndex = sequenceIndex;
	if (restartAnimation) 
	{
		m_Sequences[m_CurrentSequenceIndex].currentFrame = 0;
		m_Sequences[m_CurrentSequenceIndex].timeElapsedThisFrame = sf::Time::Zero;
	}
}
