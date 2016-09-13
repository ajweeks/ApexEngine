
#include "Player.h"
#include "ApexKeyboard.h"
#include "Tile.h"
#include "World.h"
#include "enumerations.h"
#include "ApexMain.h"
#include "TextureManager.h"
#include "AmmoDrop.h"
#include "ApexAudio.h"
#include "PhysicsActor.h"

const float Player::VEL = 550000.0f;

Player::Player(World* world, Map* map) :
	Mob(world, map, sf::Vector2f(), ActorID::PLAYER, this),
	ApexKeyListener(),
	m_World(world),
	m_SpriteSheet(TextureManager::GetTexture(TextureManager::PLAYER), 16, 32)
{
	ApexSpriteSheet::Sequence walkingSequence;
	walkingSequence.pingPongs = true;
	walkingSequence.framesLong = 3;
	walkingSequence.msPerFrame = 90;
	walkingSequence.startFrameIndex = sf::Vector2i(0, 0);
	m_SpriteSheet.AddSequence(AnimationSequence::WALKING, walkingSequence);
	
	ApexSpriteSheet::Sequence standingSequence;
	standingSequence.framesLong = 1;
	standingSequence.startFrameIndex = sf::Vector2i(1, 0);
	m_SpriteSheet.AddSequence(AnimationSequence::STANDING, standingSequence);

	Reset();
}

Player::~Player()
{
}

void Player::Reset()
{
	if (m_Actor != nullptr)
	{
		StopMoving();
	}
	m_DirFacing = DirectionFacing::RIGHT;
}

sf::Vector2f Player::GetPosition() const
{
	return m_Actor->GetPosition();
}

void Player::StopMoving()
{
	m_Actor->SetLinearVelocity(sf::Vector2f());
	m_SpriteSheet.SetCurrentSequence(AnimationSequence::STANDING);
}

void Player::CreatePhysicsActor()
{
	Entity::CreatePhysicsActor();
	m_Actor->AddCircleFixture(7.0f);
	m_Actor->AddContactListener(this);

	b2Filter collisionFilter;
	collisionFilter.categoryBits = ActorID::PLAYER;
	collisionFilter.maskBits = ActorID::BULLET | ActorID::WALL | ActorID::DOOR | ActorID::EXIT | ActorID::SHEEP;
	m_Actor->SetCollisionFilter(collisionFilter);

}

void Player::BeginContact(ApexContact* contact)
{
	switch (contact->actorA->GetUserData())
	{
	case ActorID::COIN:
	{
		ApexAudio::PlaySoundEffect(ApexAudio::Sound::COIN_PICKUP);
	} break;
	case ActorID::DOOR:
	{
	} break;
	case ActorID::EXIT:
	{
		m_ExitBuildingNextFrame = true;
	} break;
	}
}

void Player::EndContact(ApexContact* contact)
{
}

void Player::PreSolve(ApexContact* contact, bool& enableContact)
{
}

bool Player::OnKeyPress(ApexKeyboard::Key key, bool keyPressed)
{
	if (keyPressed)
	{
		switch (key) 
		{
		case ApexKeyboard::MOVE_LEFT:
		case ApexKeyboard::MOVE_RIGHT:
		case ApexKeyboard::MOVE_UP:
		case ApexKeyboard::MOVE_DOWN:
		{
			if (!m_World->IsShowingSpeechBubble())
			{
				m_SpriteSheet.SetCurrentSequence(AnimationSequence::WALKING, false);
			}
		} break;
		}
	}
	return false;
}

void Player::OnKeyRelease(ApexKeyboard::Key key)
{
}

void Player::Tick(sf::Time elapsed)
{
	if (m_BuildingIndexToEnterNextFrame != -1)
	{
		m_World->EnterMap(m_BuildingIndexToEnterNextFrame);
		m_BuildingIndexToEnterNextFrame = -1;
		return;
	}

	if (m_ExitBuildingNextFrame)
	{
		m_World->ExitMap();
		m_ExitBuildingNextFrame = false;
		return;
	}

	if (!m_World->IsShowingSpeechBubble())
	{
		HandleMovement(elapsed);
	}

	m_SpriteSheet.Tick(elapsed);
}

void Player::HandleMovement(sf::Time elapsed)
{
	const float dt = elapsed.asSeconds();
	m_SecondsElapsed += dt;
	const float dVel = VEL * dt;
	sf::Vector2f newVel = m_Actor->GetLinearVelocity();

	if (m_SpriteSheet.GetCurrentSequenceIndex() != AnimationSequence::STANDING &&
		!ApexKeyboard::IsKeyDown(ApexKeyboard::MOVE_RIGHT) &&
		!ApexKeyboard::IsKeyDown(ApexKeyboard::MOVE_LEFT) &&
		!ApexKeyboard::IsKeyDown(ApexKeyboard::MOVE_UP) &&
		!ApexKeyboard::IsKeyDown(ApexKeyboard::MOVE_DOWN))
	{
		m_SpriteSheet.SetCurrentSequence(AnimationSequence::STANDING, false);
	}

	if (ApexKeyboard::IsKeyDown(ApexKeyboard::MOVE_RIGHT))
	{
		newVel.x += dVel;
		m_DirFacing = DirectionFacing::RIGHT;
	}
	if (ApexKeyboard::IsKeyDown(ApexKeyboard::MOVE_LEFT))
	{
		newVel.x -= dVel;
		m_DirFacing = DirectionFacing::LEFT;
	}
	if (ApexKeyboard::IsKeyDown(ApexKeyboard::MOVE_UP))
	{
		newVel.y -= dVel;
	}
	if (ApexKeyboard::IsKeyDown(ApexKeyboard::MOVE_DOWN))
	{
		newVel.y += dVel;
	}

	m_Actor->SetLinearVelocity(newVel);
	//ClampPosition();
}

// Bounds check against the edges of the world (in theory shouldn't be neccessary, the world
// should have 4 containing walls)
void Player::ClampPosition()
{
	const float left = 0;
	const float right = float(m_World->GetWidth()) - 50.0f;
	const float top = 0;
	const float bottom = float(m_World->GetHeight()) - 50.0f;
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
	const float centerY = m_Actor->GetPosition().y - 8.0f;

	states.transform.translate(centerX, centerY);

	DrawShadow(target, states);
	DrawBody(target, states);
}

void Player::DrawShadow(sf::RenderTarget& target, sf::RenderStates states)
{
	static const float halfShadowImageWidth = TextureManager::GetTexture(TextureManager::SHADOW)->getSize().x / 2.0f;
	states.transform.translate(-halfShadowImageWidth, 0);
	target.draw(m_ShadowSprite, states);
}

void Player::DrawBody(sf::RenderTarget& target, sf::RenderStates states)
{
	if (m_DirFacing == DirectionFacing::LEFT)
	{
		states.transform.scale(-1, 1);
	}
	m_SpriteSheet.Draw(target, states);
}
