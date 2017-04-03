#pragma once

#include "Camera.h"
#include "BulletManager.h"
#include "Player.h"

#include "Apex\Graphics\Particle\ParticleManager.h"
#include "Apex\Graphics\SpriteSheet.h"
#include "Apex\Physics\ContactListener.h"
#include "Apex\Physics\PhysicsActorManager.h"
#include "Apex\Transition\IntTransition.h"
#include "Apex\Transition\TransformableTransition.h"
#include "Apex\Transition\Vector2fTransition.h"
#include "Apex\IO\KeyListener.h"
#include "Apex\IO\Keyboard.h"

#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderTexture.hpp>
#include <SFML\Graphics\Sprite.hpp>
#include <SFML\Window\Event.hpp>

#include <JSON\json.hpp>

class Game;
class PauseScreen;
class Minimap;
class HUD;
class Entity;
class Map;

namespace apex
{
	class StateManager;
}

class World : public apex::ContactListener, public apex::KeyListener
{
public:
	World(int worldIndex, apex::StateManager& stateManager);
	virtual ~World();

	void Tick(sf::Time elapsed);
	void Draw(sf::RenderTarget& target, sf::RenderStates states);

	void Reset();

	Player* GetPlayer();
	BulletManager& GetBulletManager();

	sf::View GetCurrentView() const;

	void JoltCamera(float xAmount, float yAmount);
	void SetScreenShake(float xScale, float yScale);

	void TogglePaused(bool pauseSounds);
	bool IsPaused() const;

	void AddParticle(apex::Particle* spriteSheet);

	bool IsShowingSpeechBubble() const;
	void SetCurrentSpeechShowing(const std::string& speech);
	void ClearSpeechShowing();

	void OnUnmappedKeypress(sf::Event::KeyEvent event);

	void EnterMap(int mapIndex);
	void ExitMap();

	int GetWorldIndex() const;

	Map* GetCurrentMap();
	void ToggleLightingEnabled();
	void SetLightingEnabled(bool enabled);
	bool IsLightingEnabled() const;

	virtual void BeginContact(apex::Contact* contact);
	virtual void EndContact(apex::Contact* contact);
	virtual void PreSolve(apex::Contact* contact, bool& enableContact);

	virtual bool OnKeyPress(apex::Keyboard::Key key, bool keyPressed) override;
	virtual void OnKeyRelease(apex::Keyboard::Key key) override;

private:
	enum class FadingOutTo
	{
		ENTER_BUILDING, EXIT_BUILDING, NONE
	};

	void SetPaused(bool paused, bool pauseSounds);

	void CreateMaps();
	void CreateMapPhysicsActors(int mapIndex);
	void DeleteMapPhysicsActors(int mapIndex);
	void DrawSpeechBubble(sf::RenderTarget& target, sf::RenderStates states);
	void CalculateSpeechBubbleInteriorSize();

	static const sf::Uint32 MILLISECONDS_PER_SPEECH_BUBBLE_LETTER;
	static const sf::Uint32 MILLISECONDS_OF_SPEECH_BUBBLE_ANIMATION;

	static const float SPEECH_BUBBLE_HIDE_SCALE;
	static const float SPEECH_BUBBLE_HIDE_Y_OFFSET;
	static const apex::Transition::EaseType SPEECH_BUBBLE_EASE_TYPE;

	bool m_LightingEnabled;
	
	int m_WorldIndex;

	Map* m_Map = nullptr;
	std::vector<Map*> m_Maps; // Buildings
	size_t m_CurrentMapShowingIndex = -1;
	size_t m_MapToTravelToIndex = -1;

	Player* m_Player = nullptr;
	Camera* m_Camera = nullptr;
	Minimap* m_Minimap = nullptr;
	HUD* m_HUD = nullptr;

	bool m_Paused;

	apex::ParticleManager m_ParticleManager;
	BulletManager* m_BulletManager = nullptr;
	PauseScreen* m_PauseScreen = nullptr;

	struct SpeechElement
	{
		std::string speechString;
		sf::Text speechText;
		sf::FloatRect bounds;

		sf::Vector2f scale;
		float frameWidth;
		float frameHeight;
		apex::Vector2fTransition interiorSize;
		sf::Vector2f speechBubbleSize;
		sf::Vector2f interiorScale;
		sf::Vector2f speechTopLeft;
	};

	bool m_IsHidingSpeechBubble;
	apex::TransformableTransition m_SpeechBubbleTransition;
	SpeechElement m_CurrentSpeech;
	apex::SpriteSheet m_SpeechBubbleSpriteSheet;

	apex::IntTransition m_SpeechLetterTransition;
	FadingOutTo m_FadingOutTo = FadingOutTo::NONE;

	apex::StateManager& m_StateManager;
};
