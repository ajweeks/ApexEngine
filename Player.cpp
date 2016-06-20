
#include "Player.h"
#include "ApexKeyboard.h"
#include "PhysicsActor.h"

Player::Player(Level* level, sf::Vector2f initalPosition) :
	Entity(Type::PLAYER), m_Level(level)
{
	Reset();
	m_GlowTexture.loadFromFile("resources/glow.png");
	m_GlowSprite.setTexture(m_GlowTexture);
	m_Actor = new PhysicsActor(initalPosition);
}

Player::~Player()
{
	delete m_Actor;
}

void Player::Reset()
{
}

sf::Vector2f Player::GetPosition() const
{
	return m_Actor->GetPosition();
}

void Player::Tick(sf::Time elapsed)
{
	m_Actor->Tick(elapsed);
	ClampPosition();
}

void Player::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	// Draw glow
	{
		sf::BlendMode prevBlendMode = states.blendMode;
		sf::Transform prevTransform = states.transform;

		states.blendMode = sf::BlendAdd;
		states.transform.translate(m_Actor->GetPosition());
		states.transform.translate(-40, -40);
		target.draw(m_GlowSprite, states);

		states.blendMode = prevBlendMode;
		states.transform = prevTransform;
	}

	sf::CircleShape circle;
	circle.setRadius(25.0f);
	circle.move(m_Actor->GetPosition());
	circle.setFillColor(sf::Color(60, 60, 195));
	target.draw(circle, states);

	circle.setRadius(21.0f);
	circle.move(4, 4);
	circle.setFillColor(sf::Color(125, 125, 255));
	target.draw(circle, states);
}

void Player::ClampPosition()
{
	const float left = 0;
	const float right = float(m_Level->GetWidth()) - 50.0f;
	const float top = 0;
	const float bottom = float(m_Level->GetHeight()) - 50.0f;
	if (m_Actor->GetPosition().x < left)
	{
		m_Actor->SetPosition(sf::Vector2f(left, m_Actor->GetPosition().y));
		m_Vel.x = -m_Vel.x;
	}
	if (m_Actor->GetPosition().x > right)
	{
		m_Actor->SetPosition(sf::Vector2f(right, m_Actor->GetPosition().y));
		m_Vel.x = -m_Vel.x;
	}
	if (m_Actor->GetPosition().y < top)
	{
		m_Actor->SetPosition(sf::Vector2f(m_Actor->GetPosition().x, top));
		m_Vel.y = -m_Vel.y;
	}
	if (m_Actor->GetPosition().y > bottom)
	{
		m_Actor->SetPosition(sf::Vector2f(m_Actor->GetPosition().x, bottom));
		m_Vel.y = -m_Vel.y;
	}
}
