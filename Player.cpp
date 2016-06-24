
#include "Player.h"
#include "ApexKeyboard.h"
#include "PhysicsActor.h"
#include "CircleFixture.h"
#include "Gun.h"

Player::Player(Level* level, sf::Vector2f initalPosition) :
	Entity(level, initalPosition, Type::PLAYER, this), 
	m_Level(level), m_IntialPos(initalPosition)
{
	m_Actor->AddFixture(new CircleFixture(m_Actor, 21.0f));

	m_GlowTexture.loadFromFile("resources/glow1.png");
	m_GlowSprite.setTexture(m_GlowTexture);

	m_Gun = new Gun(level, m_IntialPos, this);

	Reset();
}

Player::~Player()
{
	delete m_Gun;
}

void Player::Reset()
{
	m_Actor->SetPosition(m_IntialPos);
	m_Gun->Reset();
}

sf::Vector2f Player::GetPosition() const
{
	return m_Actor->GetPosition();
}

void Player::Tick(sf::Time elapsed)
{
	const float dt = elapsed.asSeconds();
	m_Elapsed += dt;
	const float dVel = 4000.0f * dt;
	sf::Vector2f newVel = m_Actor->GetVelocity();
	if (ApexKeyboard::IsKeyDown(sf::Keyboard::D) || ApexKeyboard::IsKeyDown(sf::Keyboard::Right))
		newVel.x += dVel;
	if (ApexKeyboard::IsKeyDown(sf::Keyboard::A) || ApexKeyboard::IsKeyDown(sf::Keyboard::Left))
		newVel.x -= dVel;
	if (ApexKeyboard::IsKeyDown(sf::Keyboard::W) || ApexKeyboard::IsKeyDown(sf::Keyboard::Up))
		newVel.y -= dVel;
	if (ApexKeyboard::IsKeyDown(sf::Keyboard::S) || ApexKeyboard::IsKeyDown(sf::Keyboard::Down))
		newVel.y += dVel;

	static const float FRICTION = 5.0f;
	if (newVel.x != 0 || newVel.y != 0)
	{
		newVel.x *= 1.0f - (FRICTION * dt);
		newVel.y *= 1.0f - (FRICTION * dt);

		static const float epsilon = 0.001f;
		if (abs(newVel.x) < epsilon) newVel.x = 0.0f;
		if (abs(newVel.y) < epsilon) newVel.y = 0.0f;
	}

	m_Actor->SetVelocity(newVel);

	m_GlowSprite.setColor(sf::Color(sf::Uint32(sin(m_Elapsed) * 255), sf::Uint32(cos(m_Elapsed) * 255), sf::Uint32(sin(0.5 - m_Elapsed * 2) * 255)));

	ClampPosition();

	m_Gun->Tick(elapsed);
}

void Player::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	const float radius = ((CircleFixture*)m_Actor->GetFixtures()[0])->GetRadius();
	const sf::Vector2f topLeft = m_Actor->GetPosition() + sf::Vector2f(-radius, -radius);
	// Draw glow
	{
		sf::BlendMode prevBlendMode = states.blendMode;
		sf::Transform prevTransform = states.transform;

		states.blendMode = sf::BlendAdd;
		states.transform.translate(topLeft + sf::Vector2f(-40, -40));
		target.draw(m_GlowSprite, states);

		states.blendMode = prevBlendMode;
		states.transform = prevTransform;
	}

	sf::CircleShape circle;
	circle.setRadius(21.0f);
	circle.move(topLeft);
	circle.setFillColor(sf::Color(15, 15, 25));
	target.draw(circle, states);

	circle.setRadius(18.0f);
	circle.move(3, 3);
	circle.setFillColor(sf::Color(200, 200, 215));
	target.draw(circle, states);

	m_Gun->Draw(target, states);
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
		m_Actor->SetVelocity(sf::Vector2f(0, m_Actor->GetVelocity().y));
	}
	if (m_Actor->GetPosition().x > right)
	{
		m_Actor->SetPosition(sf::Vector2f(right, m_Actor->GetPosition().y));
		m_Actor->SetVelocity(sf::Vector2f(0, m_Actor->GetVelocity().y));
	}
	if (m_Actor->GetPosition().y < top)
	{
		m_Actor->SetPosition(sf::Vector2f(m_Actor->GetPosition().x, top));
		m_Actor->SetVelocity(sf::Vector2f(m_Actor->GetVelocity().x, 0));
	}
	if (m_Actor->GetPosition().y > bottom)
	{
		m_Actor->SetPosition(sf::Vector2f(m_Actor->GetPosition().x, bottom));
		m_Actor->SetVelocity(sf::Vector2f(m_Actor->GetVelocity().x, 0));
	}
}
