#pragma once

#include "StateManager.h"
#include "FloatTransition.h"
#include "TransitionChain.h"
#include "ColorTransition.h"

#include <SFML\Graphics\Font.hpp>
#include <SFML\Graphics\Sprite.hpp>

#include <Box2D\Dynamics\b2WorldCallbacks.h>

#define APEX (ApexMain::GetSingleton())
#define ArraySize(array) (sizeof(array) / sizeof(array[0]))

class ApexDebug;
class ApexKeyListener;
class ApexMouseListener;
class ApexWindowListener;
class PhysicsActorManager;

namespace sf
{
	class RenderWindow;
}

class b2World;

// Global functions
void ApexOutputDebugString(const std::string &string);

std::vector<std::string> ApexSplit(const std::string& str);
std::vector<std::string> ApexSplit(const std::string& str, const char& delim);

sf::Color SetAlpha(const sf::Color& color, sf::Uint8 alpha);
sf::Glsl::Vec4 NormalizeColor(const sf::Color& color);

class ApexMain : public b2ContactListener
{
public:
	ApexMain();
	virtual ~ApexMain();

	ApexMain(const ApexMain&) = delete;
	ApexMain& operator=(const ApexMain&) = delete;

	static ApexMain* GetSingleton();

	static sf::Vector2f StringToVector2f(const std::string& string);
	static std::string Vector2fToString(sf::Vector2f vec);
	static std::string ColorToString(sf::Color color);
	static sf::Color StringToColor(std::string string);

	void Init();

	void Run();
	void Quit();

	sf::Vector2f GetMouseCoordsWorldSpace(sf::View view) const;
	// Returns the mouse coords relative to the game window, or (-1, -1) if mouse is outside of window
	sf::Vector2i GetMouseCoordsScreenSpace(sf::View currentView = sf::View(sf::Vector2f(0, 0), sf::Vector2f(0, 0))) const;
	bool IsMouseInWindow() const;
	void SetCursor(ApexCursor cursorType);
	void TakeScreenshot();

	StateManager* GetStateManager();
	sf::Vector2u GetWindowSize() const;

	void AddKeyListener(ApexKeyListener* keyListener, int priority = -1);
	void RemoveKeyListener(ApexKeyListener* keyListener);
	void AddMouseListener(ApexMouseListener* mouseListener);
	void RemoveMouseListener(ApexMouseListener* mouseListener);
	void AddWindowListener(ApexWindowListener* windowListener);
	void RemoveWindowListener(ApexWindowListener* windowListener);

	b2World& GetPhysicsWorld() const;
	void SetPhysicsPaused(bool physicsPaused);
	bool DEBUGIsGamePaused() const;

	void ToggleWindowFullscreen();
	void SetWindowFullscreen(bool fullscreen);
	bool IsWindowFullscreen() const;

	void ToggleVSyncEnabled();
	void SetVSyncEnabled(bool enabled);
	bool IsVSyncEnabled() const;

	sf::Time GetTimeElapsed() const;
	sf::RenderWindow* GetWindow();

	void SetSlowMoTime(sf::Time duration, ApexTransition::EaseType easeType); // Call this with the amount of time to be in slow mo for

	void StartFadeInOut(sf::Time length = FADE_IN_OUT_TIME);
	void StartFadeIn(sf::Time length = FADE_IN_OUT_TIME);
	void StartFadeOut(sf::Time length = FADE_IN_OUT_TIME);
	bool IsFadingIn();
	bool IsFadingOut();

	// Box2D overriden methods
	virtual void BeginContact(b2Contact* contact);
	virtual void EndContact(b2Contact* contact);
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

	static sf::Font FontOpenSans;
	static sf::Font FontPixelFJ8;

private:
	void CreateApexWindow(bool fullscreen);
	void Tick(double& accumulator);
	void Draw();
	void LoadCursorTextures();
	void DEBUGToggleGamePaused();

	static const sf::Time FADE_IN_OUT_TIME;
	static const std::string WINDOW_TITLE;

	static ApexMain* m_Singleton;

	sf::RenderWindow* m_Window = nullptr;
	sf::Time m_TotalElapsed;
	sf::Time m_ElapsedThisFrame;
	int m_Frames;
	int m_FPS;
	int m_Updates;
	int m_UPS;

	FloatTransition m_SlowMoData;
	TransitionChain m_FadeInOutTransitionChain;

	bool m_IsRunning;
	bool m_WindowIsFullscreen;

	// True when the world should be frozen
	bool m_PhysicsPaused = false;
	// True when the state manager shouldn't be Ticked (no animations, etc.)
	// This pauses the physics too (only a debug feature)
	bool m_DEBUG_GamePaused = false; 
	// Whether or not to show the physics fixtures
	bool m_ShowingPhysicsDebug = false;

	StateManager* m_StateManager = nullptr;
	PhysicsActorManager* m_PhysicsActorManager = nullptr;

	ApexCursor m_CursorType;
	sf::Sprite m_CursorSprite;
	std::vector<sf::Texture*> m_CursorTextures;

	std::vector<ApexKeyListener*> m_KeyListeners;
	std::vector<ApexMouseListener*> m_MouseListeners;
	std::vector<ApexWindowListener*> m_WindowListeners;

	bool m_UseVSync;
};