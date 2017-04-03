#pragma once

#include "Mob.h"
#include "Interactable.h"

#include "Apex\Graphics\SpriteSheet.h"

#include <SFML\Graphics\Text.hpp>

#include <JSON\json.hpp>

class AICharacter : public Mob, public Interactable
{
public:
	AICharacter(World& world, Map& map, const nlohmann::json& info);
	virtual ~AICharacter();

	AICharacter(const AICharacter&) = delete;
	AICharacter& operator=(const AICharacter&) = delete;

	void Tick(sf::Time elapsed) override;
	void Draw(sf::RenderTarget& target, sf::RenderStates states) override;
	virtual sf::Vector2f GetBottomMiddlePoint() override;
	virtual void CreatePhysicsActor() override;
	virtual void Interact() override;

	std::string GetCurrentSpeech() const;

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

	static const float WIDTH;
	static const float HEIGHT;

	std::vector<Statement> m_Statements;
	int m_CurrentStatementIndex;

	bool m_ShouldStop;
	bool m_ShouldRepeat;
	std::string m_Name;
	sf::Text m_NameText;
	sf::Vector2f m_Spawnpoint;
	apex::SpriteSheet m_SpriteSheet;
};
