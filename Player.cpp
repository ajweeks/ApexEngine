
#include "Player.h"
#include "ApexKeyboard.h"
#include "PhysicsActor.h"
#include "Level.h"
#include "enumerations.h"

const float Player::VEL = 500000.0f;
const float Player::MAX_VEL = 6800000.0f;
const float Player::FRICTION = 5.0f;

Player::Player(Level* level) :
	Entity(level, sf::Vector2f(), ActorID::PLAYER, this),
	m_Level(level), 
	m_SpriteSheet("resources/small-mario.png", 18, 32),
	m_IntialPos(200.0f, 75.0f),
	m_Gun(level, m_IntialPos)
{
	m_Actor->AddCircleFixture(7.0f);
	
	m_GlowTexture.loadFromFile("resources/glow_white.png");
	m_GlowSprite.setTexture(m_GlowTexture);

	SpriteSheet::Sequence walkingSequence;
	walkingSequence.framesLong = 2;
	walkingSequence.msPerFrame = 150;
	walkingSequence.startFrameIndex = sf::Vector2i(2, 0);
	m_SpriteSheet.AddSequence(int(AnimationSequence::WALKING), walkingSequence);
	
	SpriteSheet::Sequence runningSequence;
	runningSequence.framesLong = 2;
	runningSequence.msPerFrame = 100;
	runningSequence.startFrameIndex = sf::Vector2i(4, 0);
	m_SpriteSheet.AddSequence(int(AnimationSequence::RUNNING), runningSequence);
	m_SpriteSheet.SetCurrentSequence(int(AnimationSequence::WALKING));

	Reset();
}

Player::~Player()
{
}

void Player::Reset()
{
	m_Actor->SetPosition(m_IntialPos);
	m_Actor->SetLinearVelocity(sf::Vector2f(0.0f, 0.0f));
	m_Gun.Reset();
	m_IsCrouching = false;
}

sf::Vector2f Player::GetPosition() const
{
	return m_Actor->GetPosition();
}

void Player::Tick(sf::Time elapsed)
{
	HandleMovement(elapsed);

	m_GlowSprite.setColor(sf::Color(255, 255, 255, 55 + int(sin(m_SecondsElapsed * 3.5f) * 45) + 45));
	m_SpriteSheet.Tick(elapsed);

	m_Gun.Tick(elapsed);
}

void Player::HandleMovement(sf::Time elapsed)
{
	const float dt = elapsed.asSeconds();
	m_SecondsElapsed += dt;
	const float dVel = VEL * dt;
	sf::Vector2f newVel = m_Actor->GetLinearVelocity();

	if (ApexKeyboard::IsKeyPressed(sf::Keyboard::D) || ApexKeyboard::IsKeyPressed(sf::Keyboard::Right))
		m_SpriteSheet.SetCurrentSequence(int(AnimationSequence::RUNNING));
	if (ApexKeyboard::IsKeyPressed(sf::Keyboard::A) || ApexKeyboard::IsKeyPressed(sf::Keyboard::Left))
		m_SpriteSheet.SetCurrentSequence(int(AnimationSequence::WALKING));

	bool LRInput = false;
	bool UDInput = false;
	if (ApexKeyboard::IsKeyDown(sf::Keyboard::D) || ApexKeyboard::IsKeyDown(sf::Keyboard::Right))
	{
		newVel.x += dVel;
		LRInput = true;
	}
	if (ApexKeyboard::IsKeyDown(sf::Keyboard::A) || ApexKeyboard::IsKeyDown(sf::Keyboard::Left))
	{
		newVel.x -= dVel;
		LRInput = true;
	}
	if (ApexKeyboard::IsKeyDown(sf::Keyboard::W) || ApexKeyboard::IsKeyDown(sf::Keyboard::Up))
	{
		newVel.y -= dVel;
		UDInput = true;
	}
	if (ApexKeyboard::IsKeyDown(sf::Keyboard::S) || ApexKeyboard::IsKeyDown(sf::Keyboard::Down))
	{
		newVel.y += dVel;
		UDInput = true;
	}

	m_IsCrouching = false;
	if (ApexKeyboard::IsKeyDown(sf::Keyboard::C) || ApexKeyboard::IsKeyDown(sf::Keyboard::RControl))
		m_IsCrouching = true;

	const float friction = FRICTION * dt;
	if (newVel.x != 0.0f || newVel.y != 0.0f)
	{
		if (!LRInput) newVel.x *= 1.0f - friction;
		if (!UDInput) newVel.y *= 1.0f - friction;

		if (newVel.x > MAX_VEL) newVel.x = MAX_VEL;
		if (newVel.x < -MAX_VEL) newVel.x = -MAX_VEL;
		if (newVel.y > MAX_VEL) newVel.y = MAX_VEL;
		if (newVel.y < -MAX_VEL) newVel.y = -MAX_VEL;

		static const float EPSILON = 0.001f;
		if (abs(newVel.x) < EPSILON) newVel.x = 0.0f;
		if (abs(newVel.y) < EPSILON) newVel.y = 0.0f;
	}

	m_Actor->SetLinearVelocity(newVel);
	ClampPosition();
}

// Bounds check against the edges of the level (in theory shouldn't be neccessary, the levels
// should have 4 containing walls)
void Player::ClampPosition()
{
	const float left = 0;
	const float right = float(m_Level->GetWidth()) - 50.0f;
	const float top = 0;
	const float bottom = float(m_Level->GetHeight()) - 50.0f;
	if (m_Actor->GetPosition().x < left)
	{
		m_Actor->SetXPosition(left);
		m_Actor->SetXVelocity(0.0f);
	}
	if (m_Actor->GetPosition().x > right)
	{
		m_Actor->SetXPosition(right);
		m_Actor->SetXVelocity(0.0f);
	}
	if (m_Actor->GetPosition().y < top)
	{
		m_Actor->SetYPosition(top);
		m_Actor->SetYVelocity(0.0f);
	}
	if (m_Actor->GetPosition().y > bottom)
	{
		m_Actor->SetYPosition(bottom);
		m_Actor->SetYVelocity(0.0f);
	}
}

void Player::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	const float centerX = m_Actor->GetPosition().x;
	const float centerY = m_Actor->GetPosition().y;

	states.transform.translate(centerX, centerY);

	DrawGlow(target, states);
	DrawShadow(target, states);

	m_SpriteSheet.Draw(target, states);

	m_Gun.Draw(target, states);
}

void Player::DrawShadow(sf::RenderTarget& target, sf::RenderStates states)
{
	states.transform.translate(sf::Vector2f(-13, 0));
	target.draw(*m_ShadowSprite, states);
}

void Player::DrawGlow(sf::RenderTarget& target, sf::RenderStates states)
{
	states.blendMode = sf::BlendAdd;
	states.transform.translate(sf::Vector2f(-60, -50));
	target.draw(m_GlowSprite, states);
}
