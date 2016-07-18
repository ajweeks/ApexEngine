
#include "Player.h"
#include "ApexKeyboard.h"
#include "PhysicsActor.h"
#include "Level.h"
#include "enumerations.h"
#include "ApexMain.h"

const float Player::VEL = 1000000.0f;

Player::Player(Level* level) :
	Entity(level, sf::Vector2f(), ActorID::PLAYER, this),
	m_Level(level), 
	m_SpriteSheet("resources/small-mario.png", 18, 32),
	m_IntialPos(200.0f, 75.0f),
	m_Gun(level, m_IntialPos)
{
	m_Actor->AddCircleFixture(7.0f);
	m_Actor->AddContactListener(this);

	b2Filter collisionFilter;
	collisionFilter.categoryBits = ActorID::PLAYER;
	collisionFilter.maskBits = ActorID::BULLET | ActorID::WALL | ActorID::SHEEP;
	m_Actor->SetCollisionFilter(collisionFilter);

	m_GlowTexture.loadFromFile("resources/glow_white.png");
	m_GlowSprite.setTexture(m_GlowTexture);

	ApexSpriteSheet::Sequence walkingSequence;
	walkingSequence.framesLong = 2;
	walkingSequence.msPerFrame = 150;
	walkingSequence.startFrameIndex = sf::Vector2i(2, 0);
	m_SpriteSheet.AddSequence(int(AnimationSequence::WALKING), walkingSequence);
	
	ApexSpriteSheet::Sequence standingSequence;
	standingSequence.framesLong = 1;
	standingSequence.startFrameIndex = sf::Vector2i(2, 0);
	m_SpriteSheet.AddSequence(int(AnimationSequence::STANDING), standingSequence);

	Reset();
}

Player::~Player()
{
}

void Player::Reset()
{
	m_Actor->SetPosition(m_IntialPos);
	m_Actor->SetLinearVelocity(sf::Vector2f(0.0f, 0.0f));
	m_SpriteSheet.SetCurrentSequence(int(AnimationSequence::STANDING));
	m_Gun.Reset();
	m_IsCrouching = false;
}

sf::Vector2f Player::GetPosition() const
{
	return m_Actor->GetPosition();
}

Gun& Player::GetGun()
{
	return m_Gun;
}

void Player::BeginContact(PhysicsActor * thisActor, PhysicsActor * otherActor)
{
}

void Player::EndContact(PhysicsActor * thisActor, PhysicsActor * otherActor)
{
}

void Player::PreSolve(PhysicsActor * thisActor, PhysicsActor * otherActor, bool & enableContact)
{
}

void Player::Tick(sf::Time elapsed)
{
	HandleMovement(elapsed);

	m_GlowSprite.setColor(sf::Color(255, 255, 255, 55 + int(sin(m_SecondsElapsed * 7.0f) * 45) + 45));
	m_SpriteSheet.Tick(elapsed);

	m_Gun.Tick(elapsed);
}

void Player::HandleMovement(sf::Time elapsed)
{
	const float dt = elapsed.asSeconds();
	m_SecondsElapsed += dt;
	const float dVel = VEL * dt;
	sf::Vector2f newVel = m_Actor->GetLinearVelocity();

	if (ApexKeyboard::IsKeyDown(sf::Keyboard::W) || ApexKeyboard::IsKeyDown(sf::Keyboard::Up) ||
		ApexKeyboard::IsKeyDown(sf::Keyboard::A) || ApexKeyboard::IsKeyDown(sf::Keyboard::Left) ||
		ApexKeyboard::IsKeyDown(sf::Keyboard::S) || ApexKeyboard::IsKeyDown(sf::Keyboard::Down) ||
		ApexKeyboard::IsKeyDown(sf::Keyboard::D) || ApexKeyboard::IsKeyDown(sf::Keyboard::Right))
		m_SpriteSheet.SetCurrentSequence(int(AnimationSequence::WALKING), false);
	else 
		m_SpriteSheet.SetCurrentSequence(int(AnimationSequence::STANDING), false);

	if (ApexKeyboard::IsKeyDown(sf::Keyboard::D) || ApexKeyboard::IsKeyDown(sf::Keyboard::Right))
	{
		newVel.x += dVel;
	}
	if (ApexKeyboard::IsKeyDown(sf::Keyboard::A) || ApexKeyboard::IsKeyDown(sf::Keyboard::Left))
	{
		newVel.x -= dVel;
	}
	if (ApexKeyboard::IsKeyDown(sf::Keyboard::W) || ApexKeyboard::IsKeyDown(sf::Keyboard::Up))
	{
		newVel.y -= dVel;
	}
	if (ApexKeyboard::IsKeyDown(sf::Keyboard::S) || ApexKeyboard::IsKeyDown(sf::Keyboard::Down))
	{
		newVel.y += dVel;
	}

	 m_IsCrouching = (ApexKeyboard::IsKeyDown(sf::Keyboard::C) || ApexKeyboard::IsKeyDown(sf::Keyboard::RControl));

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

	DrawShadow(target, states);
	DrawGlow(target, states);

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
