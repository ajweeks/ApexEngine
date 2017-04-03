
#include "Apex\Graphics\SpriteSheet.h"

#include <SFML\Graphics\RenderTarget.hpp>

#include <assert.h>

namespace apex
{
	SpriteSheet::SpriteSheet()
	{
	}

	SpriteSheet::SpriteSheet(sf::Texture* texture, sf::Uint8 frameWidth, sf::Uint8 frameHeight)
	{
		Create(texture, frameWidth, frameHeight);
	}

	void SpriteSheet::Create(sf::Texture* texture, sf::Uint8 frameWidth, sf::Uint8 frameHeight)
	{
		assert(texture != nullptr && frameWidth != 0 && frameHeight != 0);

		m_FrameWidth = frameWidth;
		m_FrameHeight = frameHeight;

		m_Sprite.setTexture(*texture);

		m_FramesWide = texture->getSize().x / m_FrameWidth;
		m_FramesHigh = texture->getSize().y / m_FrameHeight;
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

				if (currentSequence.pingPongs)
				{
					if (currentSequence.movingForward)
					{
						if (++currentSequence.currentFrame == currentSequence.framesLong)
						{
							currentSequence.movingForward = false;
							--currentSequence.currentFrame;
						}
					}
					else
					{
						if (--currentSequence.currentFrame == 0)
						{
							currentSequence.movingForward = true;
							m_HasRestarted = true;
						}
					}
				}
				else
				{
					if (++currentSequence.currentFrame == currentSequence.framesLong)
					{
						currentSequence.currentFrame = 0;
						m_HasRestarted = true;
					}
				}
			}
		}
	}

	void SpriteSheet::Draw(sf::RenderTarget& target, sf::RenderStates states, sf::Uint8 col, sf::Uint8 row)
	{
		const sf::IntRect srcRect(col * m_FrameWidth, row * m_FrameHeight, m_FrameWidth, m_FrameHeight);
		m_Sprite.setTextureRect(srcRect);

		target.draw(m_Sprite, states);
	}

	void SpriteSheet::Draw(sf::RenderTarget& target, sf::RenderStates states)
	{
		Sequence& currentSequence = m_Sequences[m_CurrentSequenceIndex];
		int col = currentSequence.startFrameIndex.x + currentSequence.currentFrame;
		int row = currentSequence.startFrameIndex.y;
		while (col >= m_FramesWide)
		{
			col -= m_FramesWide;
			++row;
		}

		const sf::IntRect srcRect(col * m_FrameWidth, row * m_FrameHeight, m_FrameWidth, m_FrameHeight);
		m_Sprite.setTextureRect(srcRect);

		states.transform.translate(-m_FrameWidth / 2.0f, -m_FrameHeight / 2.0f);
		target.draw(m_Sprite, states);
	}

	sf::Uint8 SpriteSheet::GetCurrentFrame() const
	{
		return m_Sequences[m_CurrentSequenceIndex].currentFrame;
	}

	float SpriteSheet::GetFrameWidth() const
	{
		return m_FrameWidth;
	}

	float SpriteSheet::GetFrameHeight() const
	{
		return m_FrameHeight;
	}

	void SpriteSheet::AddSequence(sf::Uint8 sequenceIndex, Sequence sequence)
	{
		if (sequenceIndex == m_Sequences.size())
		{
			m_Sequences.push_back(sequence);
		}
		else
		{
			if (sequenceIndex > m_Sequences.size()) // Allow out of order additions (index 3, followed by index 1 for example)
			{
				m_Sequences.resize(sequenceIndex + 1);
			}
			m_Sequences[sequenceIndex] = sequence;
		}
	}

	sf::Uint8 SpriteSheet::GetCurrentSequenceIndex() const
	{
		return m_CurrentSequenceIndex;
	}

	void SpriteSheet::SetCurrentSequence(sf::Uint8 sequenceIndex, bool restartAnimation)
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

	bool SpriteSheet::HasRestarted() const
	{
		return m_HasRestarted;
	}

	void SpriteSheet::SetEntireSpriteAsOneSequence(sf::Int32 msPerFrame)
	{
		m_Sequences.clear();

		Sequence s;
		s.framesLong = m_FramesHigh * m_FramesWide;
		s.startFrameIndex = sf::Vector2i(0, 0);
		s.msPerFrame = msPerFrame;
		AddSequence(0, s);
		m_CurrentSequenceIndex = 0;
	}

	void SpriteSheet::SetFrameIndex(sf::Uint8 frameIndex)
	{
		if (frameIndex >= 0 && m_CurrentSequenceIndex < m_Sequences.size() && frameIndex < m_Sequences[m_CurrentSequenceIndex].framesLong)
		{
			m_Sequences[m_CurrentSequenceIndex].currentFrame = frameIndex;
		}
	}

	sf::Uint8 SpriteSheet::GetCurrentSequenceFramesLong() const
	{
		if (m_CurrentSequenceIndex < m_Sequences.size())
		{
			return m_Sequences[m_CurrentSequenceIndex].framesLong;
		}
		return 1;
	}


	void SpriteSheet::SetSpriteScale(const sf::Vector2f& scale)
	{
		m_Sprite.setScale(scale);
	}

	void SpriteSheet::SetSpriteScale(float scaleX, float scaleY)
	{
		m_Sprite.setScale(scaleX, scaleY);
	}

	void SpriteSheet::ResetSpriteScale()
	{
		m_Sprite.setScale(1, 1);
	}
} // namespace apex
