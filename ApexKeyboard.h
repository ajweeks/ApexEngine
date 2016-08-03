#pragma once

#include <SFML\Graphics.hpp>

#include <JSON\json.hpp>

#include <map>

class ApexKeyboard
{
public:
	enum Key
	{
		MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN,
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

	static bool GetMappedKey(int vkCode, Key& key);
	static bool GetUnMappedKey(Key key, int& vkCode);

	static void MapKey(Key key, int vkCode);

	static std::string GetSFKeyName(sf::Keyboard::Key key);
	static std::string GetKeyName(Key key);

	static size_t GetNumberOfKeys();

private:
	ApexKeyboard();
	virtual ~ApexKeyboard();

	static void StoreStringData(const std::string& fileContents);
	static int FindVKCode(const nlohmann::json& keybindings, const std::string& tagName, sf::Keyboard::Key defaultKey);

	static std::map<Key, int> m_KeysToVKCodesMap; // map Key enum values to vk codes

	static bool keysdown[sf::Keyboard::KeyCount];
	static bool keysdownLastFrame[sf::Keyboard::KeyCount];
};
