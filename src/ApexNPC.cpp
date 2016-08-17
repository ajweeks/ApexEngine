
#include "ApexNPC.h"
#include "PhysicsActor.h"
#include "TextureManager.h"
#include "ApexMain.h"
#include "World.h"

#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\Graphics\Shader.hpp>

using namespace nlohmann;

ApexNPC::ApexNPC(World* world, Map* map, sf::Vector2f position, const json& info) :
	Mob(world, map, position, ActorID::NPC)
{
	m_Actor->AddBoxFixture(10.0f, 20.0f);
	m_Actor->SetSensor(true);

	m_Name = info["name"].get<std::string>();

	std::vector<json> statements = info["statements"].get<std::vector<json>>();
	for (size_t i = 0; i < statements.size(); ++i)
	{
		Statement statement;
		std::string statementString = statements[i]["q"].get<std::string>();
		const std::string END = "[END]";
		const size_t endPos = statementString.find(END);
		const std::string REPEAT = "[REPEAT]";
		const size_t repeatPos = statementString.find(REPEAT);
		if (endPos != std::string::npos)
		{
			statement.type = Statement::Type::END;
			statementString.erase(endPos, END.length());
		}
		else if (repeatPos != std::string::npos)
		{
			statement.type = Statement::Type::REPEAT;
			statementString.erase(repeatPos, REPEAT.length());
		}

		statement.statement = statementString;
		if (statements[i].find("a") != statements[i].end())
		{
			statement.answers = statements[i]["a"].get<std::vector<std::string>>();
		}

		m_Statements.push_back(statement);
	}

	m_Sprite.setTexture(*TextureManager::GetTexture(TextureManager::NPC));
	m_NameText = sf::Text(m_Name, APEX->FontPixelFJ8, 32);
	m_NameText.setColor(sf::Color::White);
}

ApexNPC::~ApexNPC()
{
}

void ApexNPC::Tick(sf::Time elapsed)
{
}

void ApexNPC::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	const sf::Vector2f topLeft = m_Actor->GetPosition() - sf::Vector2f(8.0f, 16.0f);
	states.transform.translate(topLeft);
	target.draw(m_Sprite, states);

	states.transform.translate(-109.0f, -190.0f);
	states.transform.scale(sf::Vector2f(0.2f, 0.2f), m_Actor->GetPosition());
	states.shader = states.Default.shader;
	target.draw(m_NameText, states);
}

std::string ApexNPC::GetCurrentSpeech() const
{
	return m_Statements[m_CurrentStatementIndex].statement;
}

void ApexNPC::Interact()
{
	switch (m_Statements[m_CurrentStatementIndex].type)
	{
	case Statement::Type::REPEAT:
	{
		if (m_World->IsShowingSpeechBubble())
		{
			m_World->ClearSpeechShowing();
		}
		else
		{
			m_World->SetCurrentSpeechShowing(m_Statements[m_CurrentStatementIndex].statement);
			return;
		}
	} break;
	case Statement::Type::END:
	{
		if (m_World->IsShowingSpeechBubble())
		{
			m_World->ClearSpeechShowing();
			++m_CurrentStatementIndex;
			if (m_CurrentStatementIndex == m_Statements.size()) --m_CurrentStatementIndex;
		}
		else
		{
			m_World->SetCurrentSpeechShowing(m_Statements[m_CurrentStatementIndex].statement);
			return;
		}
	} break;
	case Statement::Type::NORMAL:
	{
		if (m_CurrentStatementIndex == m_Statements.size() - 1) // We've said all we have to say
		{
			m_World->ClearSpeechShowing(); // *drops mic*
			return;
		}

		m_World->SetCurrentSpeechShowing(m_Statements[m_CurrentStatementIndex].statement);

		++m_CurrentStatementIndex;
	} break;
	}
}
