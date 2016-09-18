
#include <APEX\Graphics\ButtonList.h>
#include <APEX\Graphics\Button.h>
#include <APEX\IO\Audio.h>
#include <APEX\IO\Mouse.h>

#include <assert.h>

namespace apex
{
	ButtonList::ButtonList()
	{
		m_Active = true;
	}

	ButtonList::~ButtonList()
	{
		for (size_t i = 0; i < m_Buttons.size(); ++i)
		{
			delete m_Buttons[i];
		}
		m_Buttons.clear();
	}

	void ButtonList::AddButton(Button* button, size_t index)
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

	Button* ButtonList::GetButton(size_t index)
	{
		return m_Buttons[index];
	}

	void ButtonList::Tick(sf::Time elapsed)
	{
		m_HighlightedButtonBackground.Tick(elapsed);
		for (size_t i = 0; i < m_Buttons.size(); ++i)
		{
			const bool wasHovering = m_Buttons[i]->IsHovering();
			m_Buttons[i]->Tick(elapsed);
			const bool isHovering = m_Buttons[i]->IsHovering();
			if (isHovering != wasHovering || Mouse::MovedLastFrame())
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

	void ButtonList::CreateBGTransition(size_t buttonIndex)
	{
		sf::Transformable start = m_HighlightedButtonBackground.GetCurrentTransformable();
		sf::Transformable end = start;
		//const float xSqueeze = 1.5f;
		//const float ySqueeze = 1.5f;
		//start.setScale(xSqueeze, ySqueeze);
		//start.move(m_Buttons[buttonIndex]->GetRectangle().getSize().x * ((1.0f - xSqueeze) / 2.0f), 0.0f);
		end.setPosition(m_Buttons[buttonIndex]->GetPosition());
		m_HighlightedButtonBackground.Create(start, end, sf::milliseconds(120), Transition::EaseType::QUADRATIC_OUT);
		Audio::PlaySoundEffect(Audio::Sound::BLIP);
	}

	void ButtonList::Draw(sf::RenderTarget& target, sf::RenderStates states)
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

	void ButtonList::Clear()
	{
		for (size_t i = 0; i < m_Buttons.size(); i++)
		{
			delete m_Buttons[i];
		}
		m_Buttons.clear();
	}

	void ButtonList::SetActive(bool active)
	{
		m_Active = active;
	}

	bool ButtonList::OnKeyPress(Keyboard::Key key, bool keyPressed)
	{
		if (!m_Active) return false;

		if (GetHighlightedButton() != nullptr)
		{
			switch (key)
			{
			case Keyboard::Key::MOVE_UP:
			{
				if (CheckHighlightedButtonNeighbor(Direction::UP)) return true;
			} break;
			case Keyboard::Key::MOVE_DOWN:
			{
				if (CheckHighlightedButtonNeighbor(Direction::DOWN)) return true;
			} break;
			case Keyboard::Key::MOVE_RIGHT:
			{
				if (CheckHighlightedButtonNeighbor(Direction::RIGHT)) return true;
			} break;
			case Keyboard::Key::MOVE_LEFT:
			{
				if (CheckHighlightedButtonNeighbor(Direction::LEFT)) return true;
			} break;
			case Keyboard::Key::INTERACT:
			{
				GetHighlightedButton()->SetActive(true);
			} break;
			}
		}
		return false;
	}

	bool ButtonList::OnUnmappedKeyPress(int vkCode, bool keyPressed)
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

	void ButtonList::OnKeyRelease(Keyboard::Key key)
	{
		if (!m_Active) return;
	}

	Button* ButtonList::GetHighlightedButton()
	{
		return m_Buttons[m_HighlightedButtonIndex];
	}

	bool ButtonList::CheckHighlightedButtonNeighbor(Direction direction)
	{
		Button* neighbor = GetHighlightedButton()->GetNeighbor(direction);
		if (neighbor != nullptr)
		{
			SetHighlightedButton(neighbor);
			m_KeyboardTouched = true;
			return true;
		}
		return false;
	}

	void ButtonList::SetHighlightedButton(Button* highlightedButton)
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

	void ButtonList::DrawBackground(sf::RenderTarget& target, sf::RenderStates states)
	{
		states.transform = m_HighlightedButtonBackground.GetCurrentTransformable().getTransform();
		sf::RectangleShape bg(m_Buttons[m_HighlightedButtonIndex]->GetRectangle().getSize());
		bg.setFillColor(Button::BG_COLOR);
		target.draw(bg, states);
	}
} // namespace apex