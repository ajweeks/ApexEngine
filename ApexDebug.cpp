
#include "ApexDebug.h"
#include "ApexMain.h"
#include "Player.h"
#include "enumerations.h"
#include "GameState.h"
#include "Level.h"
#include "PhysicsActor.h"
#include "Gun.h"

#include <stdexcept>

ApexDebug::ApexDebug()
{
	m_PlayerElementStack = CreateCollapsibleElementStack("Player", sf::Vector2f(35, 35));
	m_PlayerPosElement = AddCollapsibleElementChild(m_PlayerElementStack->m_CollapsibleElement, "pos");
	m_PlayerVelElement = AddCollapsibleElementChild(m_PlayerElementStack->m_CollapsibleElement, "vel");
	//m_PlayerGunDirectionElement = AddCollapsibleElementChild(m_PlayerElementStack->m_CollapsibleElement, "gun dir", 2);
	UpdateBackgroundRect(m_PlayerElementStack);

	if (!m_BackgroundRectangleShader.loadFromFile("resources/shaders/round_rectangle.frag", sf::Shader::Fragment))
	{
		ApexOutputDebugString("\n\n\t--Either couldn't find or couldn't compile round_rectangle.frag--\n\n\n");
	}
	else
	{
		// TODO: Call this every time the window size changes
		m_BackgroundRectangleShader.setParameter("u_resolution", static_cast<sf::Vector2f>(APEX->GetWindowSize()));
		m_BackgroundRectangleShader.setParameter("u_pos", m_PlayerElementStack->m_BackgroundRect.getPosition());
	}
}

ApexDebug::~ApexDebug()
{
	delete m_PlayerElementStack->m_CollapsibleElement;
	delete m_PlayerElementStack;
}

void ApexDebug::Tick(sf::Time elapsed)
{
	m_Elapsed += elapsed;
	if (!m_PlayerElementStack->m_CollapsibleElement->m_Collapsed)
	{
		BaseState* currentState = APEX->GetStateManager()->CurrentState();
		if (currentState->GetType() == StateType::GAME)
		{
			Level* level = ((GameState*)currentState)->GetLevel();
			Player* player = level->GetPlayer();
			std::string newPlayerPos = ApexMain::Vector2fToString(player->GetPosition());
			m_PlayerPosElement->UpdateString(newPlayerPos);

			std::string newPlayerVel = ApexMain::Vector2fToString(player->GetPhysicsActor()->GetLinearVelocity());
			m_PlayerVelElement->UpdateString(newPlayerVel);

			//float newGunDirRad = player->GetGun().GetDirection();
			//if (m_PlayerGunDirectionElement->GetCurrentStringIndex() == 0)
			//{
			//	m_PlayerGunDirectionElement->UpdateString(std::to_string(newGunDirRad) + " rad");
			//}
			//else
			//{
			//	m_PlayerGunDirectionElement->UpdateString(std::to_string(newGunDirRad * 180.0f / 3.1415f) + " deg");
			//}
		}
		UpdateBackgroundRect(m_PlayerElementStack);
	}

	APEX->SetCursor(ApexCursor::NORMAL);
	if (m_PlayerElementStack->m_CollapsibleElement->Tick(elapsed, this)) UpdateBackgroundRect(m_PlayerElementStack);

	m_BackgroundRectangleShader.setParameter("u_size", m_PlayerElementStack->m_BackgroundRect.getSize());
}

void ApexDebug::UpdateBackgroundRect(CollapsibleElementStack* stack)
{
	CollapsibleElement* parentElement = stack->m_CollapsibleElement;
	const sf::Vector2f pos = parentElement->GetPosition() + sf::Vector2f(-26, -14);
	const float stackWidth = parentElement->GetStackWidth() + 32;
	const float stackHeight = parentElement->GetStackHeight() + 20;
	stack->m_BackgroundRect = sf::RectangleShape(sf::Vector2f(stackWidth, stackHeight));
	stack->m_BackgroundRect.setPosition(pos);
	stack->m_BackgroundRect.setFillColor(sf::Color(50, 55, 60, 135));
}

void ApexDebug::Draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	DrawBackgroundRect(target, states);
	m_PlayerElementStack->m_CollapsibleElement->Draw(target, states);
}

void ApexDebug::ClearAllInput()
{
	m_PlayerElementStack->m_CollapsibleElement->ClearAllInput();
}

void ApexDebug::DrawBackgroundRect(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.shader = &m_BackgroundRectangleShader;
	target.draw(m_PlayerElementStack->m_BackgroundRect, states);
}

ApexDebug::CollapsibleElementStack* ApexDebug::CreateCollapsibleElementStack(const std::string& string, const sf::Vector2f& position, int numOfStringValues)
{
	CollapsibleElementStack* newStack = new CollapsibleElementStack();
	CollapsibleElement* newElement = new CollapsibleElement(nullptr, string);
	newElement->SetNumberOfTextValues(numOfStringValues);
	newElement->Move(position);
	newStack->m_CollapsibleElement = newElement;

	UpdateBackgroundRect(newStack);
	return newStack;
}

CollapsibleElement* ApexDebug::AddCollapsibleElementChild(CollapsibleElement* parentElement, const std::string& string, int numOfStringValues)
{
	CollapsibleElement* newElement = parentElement->AddChildElement(new CollapsibleElement(parentElement, string));
	newElement->SetNumberOfTextValues(numOfStringValues);

	return newElement;
}