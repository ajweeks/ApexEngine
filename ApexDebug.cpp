
#include "ApexDebug.h"
#include "Game.h"
#include "Player.h"
#include "enumerations.h"
#include "GameState.h"
#include "Level.h"

ApexDebug::ApexDebug()
{
	m_PlayerElementStack = CreateCollapsibleElementStack("Player", sf::Vector2f(35, 35));
	m_PlayerPosElement = AddCollapsibleElementChild(m_PlayerElementStack->m_CollapsibleElement, "pos");
	m_PlayerVelElement = AddCollapsibleElementChild(m_PlayerElementStack->m_CollapsibleElement, "vel");
	UpdateBackgroundRect(m_PlayerElementStack);

}

ApexDebug::~ApexDebug()
{
	delete m_PlayerElementStack->m_CollapsibleElement;
	delete m_PlayerElementStack;
}

void ApexDebug::Tick(sf::Time elapsed, Game* game)
{
	if (m_PlayerElementStack->m_CollapsibleElement->m_Collapsed == false)
	{
		BaseState* currentState = game->GetStateManager()->CurrentState();
		if (currentState->GetType() == StateType::GAME)
		{
			Level* level = ((GameState*)currentState)->GetLevel();
			Player* player = level->GetPlayer();
			std::string newPlayerPos = Game::Vector2fToString(player->GetPosition());
			std::string newPlayerVel = Game::Vector2fToString(player->GetVelocity());
			m_PlayerPosElement->UpdateString(newPlayerPos);
			m_PlayerVelElement->UpdateString(newPlayerVel);
		}
		UpdateBackgroundRect(m_PlayerElementStack);
	}

	game->SetCursor(sf::ApexCursor::NORMAL);
	if (m_PlayerElementStack->m_CollapsibleElement->Tick(elapsed, game, this)) UpdateBackgroundRect(m_PlayerElementStack);
}

void ApexDebug::UpdateBackgroundRect(CollapsibleElementStack* stack)
{
	CollapsibleElement* parentElement = stack->m_CollapsibleElement;
	const sf::Vector2f pos = parentElement->GetPosition() + sf::Vector2f(-25, -12);
	const float stackHeight = parentElement->GetStackHeight() + 12;
	const float stackWidth = parentElement->GetStackWidth() + 20;
	stack->m_BackgroundRect = sf::RectangleShape(sf::Vector2f(stackWidth, stackHeight));
	stack->m_BackgroundRect.setPosition(pos);
	stack->m_BackgroundRect.setFillColor(sf::Color(50, 55, 60, 135));
}

void ApexDebug::Draw(sf::RenderTarget& target) const
{
	target.draw(m_PlayerElementStack->m_BackgroundRect);
	m_PlayerElementStack->m_CollapsibleElement->Draw(target);

}

ApexDebug::CollapsibleElementStack* ApexDebug::CreateCollapsibleElementStack(const std::string& string, const sf::Vector2f& position)
{
	CollapsibleElementStack* newStack = new CollapsibleElementStack();
	CollapsibleElement* newElement  = new CollapsibleElement(nullptr, string);
	newElement->Move(position);
	newStack->m_CollapsibleElement = newElement;

	UpdateBackgroundRect(newStack);
	return newStack;
}

CollapsibleElement* ApexDebug::AddCollapsibleElementChild(CollapsibleElement* parentElement, const std::string& string)
{
	CollapsibleElement* newElement = parentElement->AddChildElement(new CollapsibleElement(parentElement, string));

	//UpdateBackgroundRect(newElement);
	return newElement;
}
