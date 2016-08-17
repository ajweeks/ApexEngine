
#include "ApexButtonList.h"
#include "ApexButton.h"

ApexButtonList::ApexButtonList()
{
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
	button->SetShowingBackground(false);
	m_Buttons[index] = button;

	if (m_Buttons.size() == 1)
	{
		CreateBGTransition(index);
	}
}

ApexButton* ApexButtonList::GetButton(size_t index)
{
	return m_Buttons[index];
}

void ApexButtonList::Tick(sf::Time elapsed)
{
	m_CurrentlyHighlightedButtonBackground.Tick(elapsed);
	for (size_t i = 0; i < m_Buttons.size(); ++i)
	{
		m_Buttons[i]->Tick(elapsed);
		if (m_Buttons[i]->IsHovering())
		{
			if (m_CurrentlyHighlightedButtonIndex != i) // A different button has been highlighted
			{
				CreateBGTransition(i);
			}

			m_CurrentlyHighlightedButtonIndex = i;
			break;
		}
	}
}

void ApexButtonList::CreateBGTransition(size_t buttonIndex)
{
	sf::Transformable start = m_CurrentlyHighlightedButtonBackground.GetCurrentTransformable();
	sf::Transformable end = start;
	const float xSqueeze = 0.6f; // Go to 60% width
	start.setScale(xSqueeze, 1.0f);
	start.move(m_Buttons[buttonIndex]->GetRectangle().getSize().x * ((1.0f - xSqueeze) / 2.0f), 0.0f);
	end.setPosition(m_Buttons[buttonIndex]->GetPosition());
	m_CurrentlyHighlightedButtonBackground.Create(start, end, sf::milliseconds(175), ApexTransition::EaseType::QUADRATIC_OUT);
}

void ApexButtonList::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	if (m_CurrentlyHighlightedButtonIndex != -1)
	{
		DrawBackground(target, states);
	}

	for (size_t i = 0; i < m_Buttons.size(); ++i)
	{
		m_Buttons[i]->Draw(target, states);
	}
}

void ApexButtonList::DrawBackground(sf::RenderTarget& target, sf::RenderStates states)
{
	states.transform = m_CurrentlyHighlightedButtonBackground.GetCurrentTransformable().getTransform();
	sf::RectangleShape bg(m_Buttons[m_CurrentlyHighlightedButtonIndex]->GetRectangle().getSize());
	bg.setFillColor(ApexButton::BG_COLOR);
	target.draw(bg, states);
}
