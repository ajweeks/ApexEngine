
#include "ApexSpriteSheet.h"

#include <SFML\Graphics\RenderTarget.hpp>

#include <assert.h>

ApexSpriteSheet::ApexSpriteSheet()
{
}

ApexSpriteSheet::ApexSpriteSheet(std::string filePath, sf::Uint8 frameWidth, sf::Uint8 frameHeight)
{
	Create(filePath, frameWidth, frameHeight);
}

void ApexSpriteSheet::Create(std::string filePath, sf::Uint8 frameWidth, sf::Uint8 frameHeight)
{
	assert(filePath != "" && frameWidth != 0 && frameHeight != 0);

	m_FrameWidth = frameWidth;
	m_FrameHeight = frameHeight;

	if (!m_Texture.loadFromFile(filePath))
	{
		throw std::invalid_argument("Could not find file: \"" + filePath + "\"");
		return;
	}
	m_Sprite.setTexture(m_Texture);

	m_FramesWide = m_Texture.getSize().x / m_FrameWidth;
	m_FramesHigh = m_Texture.getSize().y / m_FrameHeight;
}

ApexSpriteSheet::~ApexSpriteSheet()
{
}

void ApexSpriteSheet::Tick(sf::Time elapsed)
{
	if (m_CurrentSequenceIndex < m_Sequences.size())
	{
		Sequence& currentSequence = m_Sequences[m_CurrentSequenceIndex];
		currentSequence.timeElapsedThisFrame += elapsed;
		if (currentSequence.timeElapsedThisFrame.asMilliseconds() >= currentSequence.msPerFrame)
		{
			currentSequence.timeElapsedThisFrame = sf::milliseconds(
				currentSequence.timeElapsedThisFrame.asMilliseconds() - currentSequence.msPerFrame);
			if (++currentSequence.currentFrame == currentSequence.framesLong) 
			{
				currentSequence.currentFrame = 0;
				m_HasRestarted = true;
			}
		}
	}
}

void ApexSpriteSheet::Draw(sf::RenderTarget& target, sf::RenderStates states, sf::Uint8 col, sf::Uint8 row)
{
	const sf::IntRect srcRect(col * m_FrameWidth, row * m_FrameHeight, m_FrameWidth, m_FrameHeight);
	m_Sprite.setTextureRect(srcRect);

	target.draw(m_Sprite, states);
}

void ApexSpriteSheet::Draw(sf::RenderTarget& target, sf::RenderStates states)
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

	states.transform.translate(-m_FrameWidth / 2.0f, -m_FrameHeight / 2.0f);
	target.draw(m_Sprite, states);
}

sf::Uint8 ApexSpriteSheet::GetCurrentFrame() const
{
	return m_Sequences[m_CurrentSequenceIndex].currentFrame;
}

void ApexSpriteSheet::AddSequence(sf::Uint8 sequenceIndex, Sequence sequence)
{
	if (sequenceIndex == m_Sequences.size()) 
	{
		m_Sequences.push_back(sequence);
	}
	else
	{
		if (sequenceIndex > m_Sequences.size()) // Allow out of order additions (index 3, followed by index 1 for example)
		{
			m_Sequences.resize(sequenceIndex+1);
		}
		m_Sequences[sequenceIndex] = sequence;
	}
}

void ApexSpriteSheet::SetCurrentSequence(sf::Uint8 sequenceIndex, bool restartAnimation)
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

bool ApexSpriteSheet::HasRestarted() const
{
	return m_HasRestarted;
}

void ApexSpriteSheet::SetEntireSpriteAsOneSequence(sf::Int32 msPerFrame)
{
	m_Sequences.clear();

	Sequence s;
	s.framesLong = m_FramesHigh * m_FramesWide;
	s.startFrameIndex = sf::Vector2i(0, 0);
	s.msPerFrame = msPerFrame;
	AddSequence(0, s);
	m_CurrentSequenceIndex = 0;
}
