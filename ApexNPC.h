#pragma once

#include "Mob.h"
#include "Interactable.h"

#include <SFML\Graphics\Text.hpp>

#include <JSON\json.hpp>

#include <string>

class ApexNPC : public Mob, public Interactable
{
public:
	ApexNPC(Level* level, sf::Vector2f position, const nlohmann::json& info);
	virtual ~ApexNPC();

	ApexNPC(const ApexNPC&) = delete;
	ApexNPC& operator=(const ApexNPC&) = delete;

	void Tick(sf::Time elapsed) override;
	void Draw(sf::RenderTarget& target, sf::RenderStates states) override;

	std::string GetCurrentSpeech() const;
	void Reply(int responseIndex);

	virtual void Interact() override;

private:
	struct Question
	{
		std::string question;
		std::vector<std::string> answers; // Can be empty (for statements)
		int answerIndex = -1;
	};

	std::string m_Name;
	std::vector<Question> m_Statments;
	int m_CurrentStatementIndex;

	sf::Sprite m_Sprite;
	sf::Text m_NameText;
};
