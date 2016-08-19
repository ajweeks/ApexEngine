#pragma once

#include "Mob.h"
#include "Interactable.h"

#include <SFML\Graphics\Text.hpp>

#include <JSON\json.hpp>

class ApexNPC : public Mob, public Interactable
{
public:
	ApexNPC(World* world, Map* map, const nlohmann::json& info);
	virtual ~ApexNPC();

	ApexNPC(const ApexNPC&) = delete;
	ApexNPC& operator=(const ApexNPC&) = delete;

	void Tick(sf::Time elapsed) override;
	void Draw(sf::RenderTarget& target, sf::RenderStates states) override;

	std::string GetCurrentSpeech() const;

	virtual void CreatePhysicsActor(ApexContactListener* b2ContactListener) override;

	virtual void Interact() override;

private:
	struct Statement
	{
		enum class Type
		{
			NORMAL, END, REPEAT
		};

		std::string statement;
		Type type = Type::NORMAL;
		std::vector<std::string> answers; // Can be empty
		int answerIndex;
	};

	std::vector<Statement> m_Statements;
	int m_CurrentStatementIndex;

	bool m_ShouldStop;
	bool m_ShouldRepeat;
	std::string m_Name;
	sf::Text m_NameText;
	sf::Vector2f m_Spawnpoint;
	sf::Sprite m_Sprite;
};
