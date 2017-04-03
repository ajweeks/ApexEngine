
#include "Game\World\Player.h"
#include "Game\World\Tile.h"
#include "Game\World\World.h"
#include "Game\World\AmmoDrop.h"
#include "Game\World\Building.h"
#include "Game\World\Map.h"
#include "Game\Game.h"
#include "Game\Debug\ImGui.h"

#include "Apex\IO\Keyboard.h"
#include "Apex\Graphics\TextureManager.h"
#include "Apex\IO\Audio.h"
#include "Apex\Physics\PhysicsActor.h"

const float Player::VEL = 500000.0f;
const float Player::WIDTH = 12.0f;
const float Player::HEIGHT = 28.0f;

Player::Player(World& world, Map& map) :
	Mob(world, map, sf::Vector2f(), ActorID::PLAYER, this),
	apex::KeyListener(),
	m_SpriteSheet(apex::TextureManager::GetTexture(Texture::PLAYER), 20, 32)
{
	apex::SpriteSheet::Sequence walkingDownSequence;
	walkingDownSequence.framesLong = 4;
	walkingDownSequence.msPerFrame = 120;
	walkingDownSequence.startFrameIndex = sf::Vector2i(0, 0);
	m_SpriteSheet.AddSequence(AnimationSequence::WALKING_DOWN, walkingDownSequence);

	apex::SpriteSheet::Sequence walkingUpSequence;
	walkingUpSequence.framesLong = 4;
	walkingUpSequence.msPerFrame = 120;
	walkingUpSequence.startFrameIndex = sf::Vector2i(4, 0);
	m_SpriteSheet.AddSequence(AnimationSequence::WALKING_UP, walkingUpSequence);

	apex::SpriteSheet::Sequence walkingSidewaysSequence;
	walkingSidewaysSequence.framesLong = 4;
	walkingSidewaysSequence.msPerFrame = 120;
	walkingSidewaysSequence.startFrameIndex = sf::Vector2i(0, 1);
	m_SpriteSheet.AddSequence(AnimationSequence::WALKING_SIDEWAYS, walkingSidewaysSequence);

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

int Player::GetDoorIndexTouching() const
{
	return m_DoorIndexTouching;
}

void Player::StopMoving()
{
	m_Actor->SetLinearVelocity(sf::Vector2f());
	m_StandingStill = true;
	m_SpriteSheet.SetFrameIndex(0);
}

void Player::CreatePhysicsActor()
{
	Mob::CreatePhysicsActor();
	m_BodyFixture = m_Actor->AddBoxFixture(WIDTH, HEIGHT, true, b2Vec2(0.0f, -19.0f));
	m_FootFixture = m_Actor->AddCircleFixture(2.6f, false, b2Vec2(0.0f, -6.8f), 1.0f, 0.0f);
	m_Actor->AddContactListener(this);

	b2Filter collisionFilter;
	collisionFilter.categoryBits = int(ActorID::PLAYER);
	collisionFilter.maskBits = int(ActorID::BULLET) | int(ActorID::BED) | int(ActorID::BUILDING) | 
		int(ActorID::DOOR) | int(ActorID::EXIT) | int(ActorID::SHEEP) | int(ActorID::COIN) | int(ActorID::SOLID_BLOCK);
	m_Actor->SetCollisionFilter(collisionFilter);
}

void Player::BeginContact(apex::Contact* contact)
{
	switch (ActorID(contact->actorA->GetUserData()))
	{
	case ActorID::COIN:
	{
		apex::Audio::PlaySoundEffect(size_t(Sound::COIN_PICKUP));
	} break;
	case ActorID::BUILDING:
	{
		Building* building = static_cast<Building*>(contact->actorA->GetUserPointer());
		if (contact->fixtureB == m_FootFixture && contact->fixtureA == building->GetDoorFixture())
		{
			m_DoorIndexTouching = building->GetIndex();
		}
	} break;
	case ActorID::EXIT:
	{
		m_ExitBuildingNextFrame = true;
	} break;
	}
}

void Player::EndContact(apex::Contact* contact)
{
	switch (ActorID(contact->actorA->GetUserData()))
	{
	case ActorID::BUILDING:
	{
		m_DoorIndexTouching = -1;
	} break;
	}
}

void Player::PreSolve(apex::Contact* contact, bool& enableContact)
{
}

bool Player::OnKeyPress(apex::Keyboard::Key key, bool keyPressed)
{
	if (keyPressed)
	{
		switch (Key(key.vkCode)) 
		{
		case Key::MOVE_LEFT:
		{
			if (!m_World.IsShowingSpeechBubble())
			{
				m_StandingStill = false;
			}
		} break;
		case Key::MOVE_RIGHT:
		{
			if (!m_World.IsShowingSpeechBubble())
			{
				m_StandingStill = false;
			}
		} break;
		case Key::MOVE_UP:
		{
			if (!m_World.IsShowingSpeechBubble())
			{
				m_StandingStill = false;
			}
		} break;
		case Key::MOVE_DOWN:
		{
			if (!m_World.IsShowingSpeechBubble())
			{
				m_StandingStill = false;
			}
		} break;
		case Key::INTERACT:
		{
			if (m_DoorIndexTouching != -1 && !m_Map.GetBuildings()[m_DoorIndexTouching]->IsDoorLocked())
			{
				m_BuildingIndexToEnterNextFrame = m_DoorIndexTouching;
				m_DoorIndexTouching = -1;
				return true;
			}
		} break;
		}
	}
	return false;
}

void Player::OnKeyRelease(apex::Keyboard::Key key)
{
}

sf::Vector2f Player::GetBottomMiddlePoint()
{
	return m_Actor->GetPosition() + sf::Vector2f(0, HEIGHT / 2.0f);
}

void Player::Tick(sf::Time elapsed)
{
	if (m_BuildingIndexToEnterNextFrame != -1)
	{
		m_World.EnterMap(m_BuildingIndexToEnterNextFrame);
		m_BuildingIndexToEnterNextFrame = -1;
		return;
	}

	if (m_ExitBuildingNextFrame)
	{
		m_World.ExitMap();
		m_ExitBuildingNextFrame = false;
		return;
	}

	if (!m_World.IsShowingSpeechBubble())
	{
		HandleMovement(elapsed);
	}

	if (!m_StandingStill)
	{
		m_SpriteSheet.Tick(elapsed);
	}

	if (Game::IsShowingDebugOverlay())
	{
		if (ImGui::CollapsibleElement("Player"))
		{
			ImGui::Text("Player pos: " + ImGui::ToString(m_Actor->GetPosition()));
			ImGui::Text("Player vel: " + ImGui::ToString(m_Actor->GetLinearVelocity()));
		}
		ImGui::CollapsibleElementPop();
	}
}

void Player::HandleMovement(sf::Time elapsed)
{
	const float dt = elapsed.asSeconds();
	m_SecondsElapsed += dt;
	const float dVel = VEL * dt;
	sf::Vector2f newVel = m_Actor->GetLinearVelocity();

	if (!m_StandingStill &&
		!apex::Keyboard::IsKeyDown(size_t(Key::MOVE_RIGHT)) &&
		!apex::Keyboard::IsKeyDown(size_t(Key::MOVE_LEFT)) &&
		!apex::Keyboard::IsKeyDown(size_t(Key::MOVE_UP)) &&
		!apex::Keyboard::IsKeyDown(size_t(Key::MOVE_DOWN)))
	{
		StopMoving();
	}

	if (apex::Keyboard::IsKeyDown(size_t(Key::MOVE_UP)))
	{
		newVel.y -= dVel;
		m_DirFacing = DirectionFacing::UP;
		m_SpriteSheet.SetCurrentSequence(AnimationSequence::WALKING_UP, false);
	}
	else if (apex::Keyboard::IsKeyDown(size_t(Key::MOVE_DOWN)))
	{
		newVel.y += dVel;
		m_DirFacing = DirectionFacing::DOWN;
		m_SpriteSheet.SetCurrentSequence(AnimationSequence::WALKING_DOWN, false);
	}
	if (apex::Keyboard::IsKeyDown(size_t(Key::MOVE_RIGHT)))
	{
		newVel.x += dVel;
		m_DirFacing = DirectionFacing::RIGHT;
		m_SpriteSheet.SetCurrentSequence(AnimationSequence::WALKING_SIDEWAYS, false);
	}
	else if (apex::Keyboard::IsKeyDown(size_t(Key::MOVE_LEFT)))
	{
		newVel.x -= dVel;
		m_DirFacing = DirectionFacing::LEFT;
		m_SpriteSheet.SetCurrentSequence(AnimationSequence::WALKING_SIDEWAYS, false);
	}

	m_Actor->SetLinearVelocity(newVel);
	//ClampPosition();
}

// Bounds check against the edges of the world (in theory shouldn't be neccessary, the world
// should have 4 containing walls)
void Player::ClampPosition()
{
	Map* currentMap = m_World.GetCurrentMap();
	const size_t tileSize = currentMap->GetTileSize();
	const float left = 0;
	const float right = float(currentMap->GetTilesWide() * tileSize) - 50.0f;
	const float top = 0;
	const float bottom = float(currentMap->GetTilesHigh() * tileSize) - 50.0f;
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
	const float centerY = m_Actor->GetPosition().y - 20.0f;

	states.transform.translate(centerX, centerY);
	DrawShadow(target, states);
	DrawBody(target, states);
}

void Player::DrawBody(sf::RenderTarget& target, sf::RenderStates states)
{
	if (m_DirFacing == DirectionFacing::LEFT)
	{
		states.transform.scale(-1, 1);
	}
	m_SpriteSheet.Draw(target, states);
}
