#pragma once

#include <APEX\Transition\Transition.h>
#include <APEX\Transition\FloatTransition.h>
#include <APEX\Transition\TransitionChain.h>
#include <APEX\Transition\ColorTransition.h>
#include <APEX\enumerations.h>

#include <SFML\Graphics\Glsl.hpp>
#include <SFML\Graphics\Font.hpp>
#include <SFML\Graphics\Sprite.hpp>

#include <Box2D\Dynamics\b2WorldCallbacks.h>

#define APEX (apex::ApexMain::GetSingleton())
#define ArraySize(array) (sizeof(array) / sizeof(array[0]))

namespace sf
{
	class RenderWindow;
}

namespace apex
{
	class KeyListener;
	class MouseListener;
	class WindowListener;
	class PhysicsActorManager;
	class AbstractGame;
}

class b2World;

namespace apex
{
	// Global functions
	enum class LogType
	{
		LOG_NONE, LOG_INFO, LOG_WARNING, LOG_ERROR
	};
	void PrintString(const std::string &string, LogType logType = LogType::LOG_NONE);

	std::vector<std::string> Split(const std::string& str);
	std::vector<std::string> Split(const std::string& str, const char& delim);

	sf::Color SetAlpha(const sf::Color& color, sf::Uint8 alpha);
	sf::Glsl::Vec4 NormalizeColor(const sf::Color& color);

	sf::Vector2f StringToVector2f(const std::string& string);
	std::string Vector2fToString(sf::Vector2f vec);
	std::string ColorToString(sf::Color color);
	sf::Color StringToColor(std::string string);

	class ApexMain : public b2ContactListener
	{
	public:
		ApexMain();
		virtual ~ApexMain();

		ApexMain(const ApexMain&) = delete;
		ApexMain& operator=(const ApexMain&) = delete;

		static ApexMain* GetSingleton();

		void Run(AbstractGame* game);
		void Quit();

		sf::Vector2f GetMouseCoordsWorldSpace(sf::View view) const;
		// Returns the mouse coords relative to the game window, or (-1, -1) if mouse is outside of window
		sf::Vector2i GetMouseCoordsScreenSpace(sf::View currentView = sf::View(sf::Vector2f(0, 0), sf::Vector2f(0, 0))) const;
		bool IsMouseInWindow() const;
		void SetCursor(Cursor cursorType);
		void TakeScreenshot();

		sf::Vector2u GetWindowSize() const;

		void AddKeyListener(KeyListener* keyListener, int priority = -1);
		void RemoveKeyListener(KeyListener* keyListener);
		void AddMouseListener(MouseListener* mouseListener);
		void RemoveMouseListener(MouseListener* mouseListener);
		void AddWindowListener(WindowListener* windowListener);
		void RemoveWindowListener(WindowListener* windowListener);

		b2World& GetPhysicsWorld() const;
		void SetPhysicsPaused(bool physicsPaused);
		bool DEBUGIsApexMainPaused() const;

		void ToggleWindowFullscreen();
		void SetWindowFullscreen(bool fullscreen);
		bool IsWindowFullscreen() const;

		void ToggleVSyncEnabled();
		void SetVSyncEnabled(bool enabled);
		bool IsVSyncEnabled() const;

		sf::Time GetTimeElapsed() const;
		sf::RenderWindow* GetWindow();

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
		void AddCursorType(Cursor cursorType, sf::Texture* texture);
		void SetDefaultCursorVisible(bool visible);
		void DEBUGToggleGamePaused();

		static const std::string WINDOW_TITLE;

		static ApexMain* s_Singleton;
		static AbstractGame* s_Game;

		sf::RenderWindow* m_Window = nullptr;

		sf::Time m_TotalElapsed;
		sf::Time m_ElapsedThisFrame;
		int m_Frames;
		int m_FPS;
		int m_Updates;
		int m_UPS;

		bool m_IsRunning;
		bool m_WindowIsFullscreen;

		// True when the world should be frozen
		bool m_PhysicsPaused = false;
		// True when the state manager shouldn't be Ticked (no animations, etc.)
		// This pauses the physics too (only a debug feature)
		bool m_DEBUG_ApexMainPaused = false; 
		// Whether or not to show the physics fixtures
		bool m_ShowingPhysicsDebug = false;

		PhysicsActorManager* m_PhysicsActorManager = nullptr;

		bool m_CustomCursorVisible;
		Cursor m_CursorType;
		sf::Sprite m_CursorSprite;
		std::vector<sf::Texture*> m_CursorTextures;

		std::vector<KeyListener*> m_KeyListeners;
		std::vector<MouseListener*> m_MouseListeners;
		std::vector<WindowListener*> m_WindowListeners;

		bool m_UseVSync;
	};
} // namespace apex
