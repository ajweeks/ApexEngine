
#include "ApexButtonList.h"
#include "ApexButton.h"
#include "ApexAudio.h"
#include "ApexMouse.h"

#include <assert.h>

ApexButtonList::ApexButtonList()
{
	m_Active = true;
}

ApexButtonList::~ApexButtonList()
{
	for (size_t i = 0; i < m_Buttons.size(); ++i)
	{
		delete m_Buttons[i];
	}
	m_Buttons.clear();
}

void ApexButtonList::AddButton(ApexButton* button, size_t index)
{
	assert(m_Buttons[index] == nullptr);

	button->SetShowingBackground(false);
	m_Buttons[index] = button;

	if (m_Buttons.size() == 1)
	{
		CreateBGTransition(index);
	}

	if (index > 0)
	{
		button->SetNeighbor(m_Buttons[index - 1], Direction::UP);
		m_Buttons[index - 1]->SetNeighbor(button, Direction::DOWN);
		button->SetNeighbor(m_Buttons[0], Direction::DOWN);
		m_Buttons[0]->SetNeighbor(button, Direction::UP);
	}
}

ApexButton* ApexButtonList::GetButton(size_t index)
{
	return m_Buttons[index];
}

void ApexButtonList::Tick(sf::Time elapsed)
{
	m_HighlightedButtonBackground.Tick(elapsed);
	for (size_t i = 0; i < m_Buttons.size(); ++i)
	{
		const bool wasHovering = m_Buttons[i]->IsHovering();
		m_Buttons[i]->Tick(elapsed);
		const bool isHovering = m_Buttons[i]->IsHovering();
		if (isHovering != wasHovering || ApexMouse::MovedLastFrame())
		{
			m_KeyboardTouched = false;
		}

		if (isHovering && !m_KeyboardTouched)
		{
			if (m_HighlightedButtonIndex != i) // A different button has been highlighted
			{
				CreateBGTransition(i);
			}

			m_HighlightedButtonIndex = i;
			break;
		}
	}
}

void ApexButtonList::CreateBGTransition(size_t buttonIndex)
{
	sf::Transformable start = m_HighlightedButtonBackground.GetCurrentTransformable();
	sf::Transformable end = start;
	//const float xSqueeze = 1.5f;
	//const float ySqueeze = 1.5f;
	//start.setScale(xSqueeze, ySqueeze);
	//start.move(m_Buttons[buttonIndex]->GetRectangle().getSize().x * ((1.0f - xSqueeze) / 2.0f), 0.0f);
	end.setPosition(m_Buttons[buttonIndex]->GetPosition());
	m_HighlightedButtonBackground.Create(start, end, sf::milliseconds(120), ApexTransition::EaseType::QUADRATIC_OUT);
	ApexAudio::PlaySoundEffect(ApexAudio::Sound::BLIP);
}

void ApexButtonList::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	if (m_HighlightedButtonIndex != -1)
	{
		DrawBackground(target, states);
	}

	for (size_t i = 0; i < m_Buttons.size(); ++i)
	{
		m_Buttons[i]->Draw(target, states);
	}
}

void ApexButtonList::Clear()
{
	for (size_t i = 0; i < m_Buttons.size(); i++)
	{
		delete m_Buttons[i];
	}
	m_Buttons.clear();
}

void ApexButtonList::SetActive(bool active)
{
	m_Active = active;
}

bool ApexButtonList::OnKeyPress(ApexKeyboard::Key key, bool keyPressed)
{
	if (!m_Active) return false;

	if (GetHighlightedButton() != nullptr)
	{
		switch (key)
		{
		case ApexKeyboard::Key::MOVE_UP:
		{
			if (CheckHighlightedButtonNeighbor(Direction::UP)) return true;
		} break;
		case ApexKeyboard::Key::MOVE_DOWN:
		{
			if (CheckHighlightedButtonNeighbor(Direction::DOWN)) return true;
		} break;
		case ApexKeyboard::Key::MOVE_RIGHT:
		{
			if (CheckHighlightedButtonNeighbor(Direction::RIGHT)) return true;
		} break;
		case ApexKeyboard::Key::MOVE_LEFT:
		{
			if (CheckHighlightedButtonNeighbor(Direction::LEFT)) return true;
		} break;
		case ApexKeyboard::Key::INTERACT:
		{
			GetHighlightedButton()->SetActive(true);
		} break;
		}
	}
	return false;
}

bool ApexButtonList::OnUnmappedKeyPress(int vkCode, bool keyPressed)
{
	if (!m_Active) return false;

	switch (vkCode)
	{
	case sf::Keyboard::Key::Up:
	{
		if (CheckHighlightedButtonNeighbor(Direction::UP)) return true;
	} break;
	case sf::Keyboard::Key::Down:
	{
		if (CheckHighlightedButtonNeighbor(Direction::DOWN)) return true;
	} break;
	case sf::Keyboard::Key::Left:
	{
		if (CheckHighlightedButtonNeighbor(Direction::LEFT)) return true;
	} break;
	case sf::Keyboard::Key::Right:
	{
		if (CheckHighlightedButtonNeighbor(Direction::RIGHT)) return true;
	} break;
	}
	return false;
}

void ApexButtonList::OnKeyRelease(ApexKeyboard::Key key)
{
	if (!m_Active) return;
}

ApexButton* ApexButtonList::GetHighlightedButton()
{
	return m_Buttons[m_HighlightedButtonIndex];
}

bool ApexButtonList::CheckHighlightedButtonNeighbor(Direction direction)
{
	ApexButton* neighbor = GetHighlightedButton()->GetNeighbor(direction);
	if (neighbor != nullptr)
	{
		SetHighlightedButton(neighbor);
		m_KeyboardTouched = true;
		return true;
	}
	return false;
}

void ApexButtonList::SetHighlightedButton(ApexButton* highlightedButton)
{
	for (size_t i = 0; i < m_Buttons.size(); i++)
	{
		if (highlightedButton == m_Buttons[i])
		{
			m_HighlightedButtonIndex = i;
			CreateBGTransition(m_HighlightedButtonIndex);
			return;
		}
	}
	assert(false);
}

void ApexButtonList::DrawBackground(sf::RenderTarget& target, sf::RenderStates states)
{
	states.transform = m_HighlightedButtonBackground.GetCurrentTransformable().getTransform();
	sf::RectangleShape bg(m_Buttons[m_HighlightedButtonIndex]->GetRectangle().getSize());
	bg.setFillColor(ApexButton::BG_COLOR);
	target.draw(bg, states);
}
