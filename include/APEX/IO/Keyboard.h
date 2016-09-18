#pragma once

#include <SFML\Graphics.hpp>

#include <JSON\json.hpp>

#include <map>

namespace apex
{
	class Keyboard
	{
	public:
		enum Key
		{
			MOVE_UP, MOVE_LEFT, MOVE_DOWN, MOVE_RIGHT,
			PAUSE, INTERACT, SCREENSHOT,
			DEBUG_TOGGLE_INFO_OVERLAY, DEBUG_TOGGLE_LIGHT_EDITOR, DEBUG_RESTART,
			DEBUG_TOGGLE_PHYSICS_OVERLAY, DEBUG_STEP_ONE_PHYSICS_FRAME, DEBUG_PAUSE_EVERYTHING,

			NONE
		};

		static void LoadKeybindingsFromFile();
		static void SaveKeybindingsToFile();
		static void ResetDefaultValues();

		static void Tick();
		static void Clear();
		static bool IsKeyDown(Key key);
		static bool IsKeyPressed(Key key);
		static bool IsKeyReleased(Key key);

		static bool GetKeyFromVKCode(int vkCode, Key& key);
		static bool GetVKCodeFromKey(Key key, int& vkCode);

		static void MapKey(Key key, int vkCode);

		static std::string GetSFKeyName(sf::Keyboard::Key key);
		static std::string GetKeyName(Key key);

		static size_t GetNumberOfKeys();

	private:
		Keyboard();
		virtual ~Keyboard();

		// TODO: Allow mouse/controller input to replace certain keyboard keys
		static void StoreStringData(const std::string& fileContents);
		static int FindVKCode(const nlohmann::json& keybindings, const std::string& tagName, sf::Keyboard::Key defaultKey);

		static std::map<Key, int> m_KeysToVKCodesMap; // map Key enum values to vk codes

		static bool keysdown[sf::Keyboard::KeyCount];
		static bool keysdownLastFrame[sf::Keyboard::KeyCount];
	};
} // namespace apex
