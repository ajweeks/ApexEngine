
#include "ApexNPC.h"
#include "PhysicsActor.h"
#include "TextureManager.h"
#include "ApexMain.h"
#include "World.h"

#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\Graphics\Shader.hpp>

using namespace nlohmann;

const float ApexNPC::WIDTH = 10.0f;
const float ApexNPC::HEIGHT = 28.0f;

ApexNPC::ApexNPC(World& world, Map& map, const json& info) :
	Mob(world, map, sf::Vector2f(), ActorID::NPC, this)
{
	m_Spawnpoint = ApexMain::StringToVector2f(info["spawn_point"]["position"].get<std::string>());
	m_Position = m_Spawnpoint;

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

	m_SpriteSheet.Create(TextureManager::GetTexture(TextureManager::NPC), 16, 32);

	ApexSpriteSheet::Sequence standingSequence;
	m_SpriteSheet.AddSequence(0, standingSequence);
	m_SpriteSheet.SetCurrentSequence(0);

	m_Name = info["name"].get<std::string>();
	m_NameText = sf::Text(m_Name, APEX->FontPixelFJ8, 48);
	m_NameText.setFillColor(sf::Color::White);
}

ApexNPC::~ApexNPC()
{
}

void ApexNPC::Tick(sf::Time elapsed)
{
	m_SpriteSheet.Tick(elapsed);
}

void ApexNPC::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	states.transform.translate(m_Actor->GetPosition());
	m_SpriteSheet.Draw(target, states);

	states.shader = states.Default.shader;

	const float TEXT_WIDTH = m_NameText.getLocalBounds().width;
	const float TEXT_SCALE = 0.15f;
	states.transform.translate(-TEXT_WIDTH * TEXT_SCALE / 2.0f, 15)
		.scale(sf::Vector2f(TEXT_SCALE, TEXT_SCALE));
	target.draw(m_NameText, states);

}

sf::Vector2f ApexNPC::GetBottomMiddlePoint()
{
	// TODO: Fix player appearing in front of npcs
	return m_Actor->GetPosition() + sf::Vector2f(0, HEIGHT / 2.0f);
}

std::string ApexNPC::GetCurrentSpeech() const
{
	return m_Statements[m_CurrentStatementIndex].statement;
}

void ApexNPC::CreatePhysicsActor()
{
	if (m_Actor == nullptr)
	{
		// TODO: Fix player-npc clipping (figure out how to set Physics Actors' origin point)
		Mob::CreatePhysicsActor();
		m_Actor->AddBoxFixture(WIDTH, HEIGHT, true);
	}
}

void ApexNPC::Interact()
{
	if (m_ShouldStop)
	{
		m_ShouldStop = false;
		m_World.ClearSpeechShowing();
		++m_CurrentStatementIndex;
		return;
	}
	else if (m_ShouldRepeat)
	{
		if (m_World.IsShowingSpeechBubble())
		{
			m_World.ClearSpeechShowing();
		}
		else
		{
			m_World.SetCurrentSpeechShowing(m_Statements[m_CurrentStatementIndex].statement);
		}
		return;
	}

	switch (m_Statements[m_CurrentStatementIndex].type)
	{
	case Statement::Type::REPEAT:
	{
		m_World.SetCurrentSpeechShowing(m_Statements[m_CurrentStatementIndex].statement);
		m_ShouldRepeat = true;
	} break;
	case Statement::Type::END:
	{
		m_World.SetCurrentSpeechShowing(m_Statements[m_CurrentStatementIndex].statement);
		m_ShouldStop = true;
	} break;
	case Statement::Type::NORMAL:
	{
		if (m_CurrentStatementIndex == m_Statements.size() - 1) // We've said all we have to say
		{
			m_World.ClearSpeechShowing(); // *drops mic*
		}

		m_World.SetCurrentSpeechShowing(m_Statements[m_CurrentStatementIndex].statement);

		++m_CurrentStatementIndex;
	} break;
	}
}
