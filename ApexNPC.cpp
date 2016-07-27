
#include "ApexNPC.h"
#include "PhysicsActor.h"
#include "TextureManager.h"
#include "ApexMain.h"
#include "Level.h"

#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\Graphics\Shader.hpp>

using namespace nlohmann;

ApexNPC::ApexNPC(Level* level, sf::Vector2f position, const json& info) :
	Mob(level, position, ActorID::NPC)
{
	m_Actor->AddBoxFixture(10.0f, 20.0f);
	m_Actor->SetSensor(true);

	m_Name = info["name"].get<std::string>();

	std::vector<json> questions = info["speech"].get<std::vector<json>>();
	for (size_t i = 0; i < questions.size(); ++i)
	{
		Question question;

		if (questions[i].find("answers") != questions[i].end())
		{
			question.question = questions[i]["question"].get<std::string>();
			question.answers = questions[i]["answers"].get<std::vector<std::string>>();
		}
		else
		{
			question.question = questions[i].get<std::string>();
		}

		m_Statments.push_back(question);
	}
	m_CurrentStatementIndex = -1;

	m_Sprite.setTexture(*TextureManager::GetTexture(TextureManager::NPC));
	m_NameText = sf::Text(m_Name, APEX->FontOpenSans, 16);
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

	states.transform = states.Default.transform;
	states.transform.scale(0.5f, 0.5f, m_Actor->GetPosition().x, m_Actor->GetPosition().y);
	states.transform.translate(topLeft).translate(-10.0f, 55.0f);
	states.shader = states.Default.shader;
	target.draw(m_NameText, states);
}

std::string ApexNPC::GetCurrentSpeech() const
{
	return m_Statments[m_CurrentStatementIndex].question;
}

void ApexNPC::Reply(int responseIndex)
{

	++m_CurrentStatementIndex;
}

void ApexNPC::Interact()
{
	++m_CurrentStatementIndex;

	if (m_CurrentStatementIndex == m_Statments.size())
	{
		m_CurrentStatementIndex = -1;
		m_Level->ClearSpeechShowing();
	}
	else
	{
		m_Level->SetCurrentSpeechShowing(m_Statments[m_CurrentStatementIndex].question);
	}
}
