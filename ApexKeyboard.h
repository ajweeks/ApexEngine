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
		DEBUG_TOGGLE_DEBUG_OVERLAY, DEBUG_TOGGLE_LIGHT_EDITOR, DEBUG_RESTART,

		_LAST_ELEMENT
	};
	static std::string names[];

	static void LoadKeybindingsFromFile();
	static void SaveKeybindingsToFile();
	static void ResetDefaultValues();

	static void Tick();
	static void Clear();
	static bool IsKeyDown(Key key);
	static bool IsKeyPressed(Key key);
	static bool IsKeyReleased(Key key);

	static bool GetMappedKey(int vkCode, Key& key);

private:
	ApexKeyboard();
	virtual ~ApexKeyboard();

	static void StoreStringData(const std::string& fileContents);
	static int FindVKCode(const nlohmann::json& keybindings, const std::string& tagName, sf::Keyboard::Key defaultKey);

	static std::string GetTagContent(const std::string& totalString, const std::string& tagString, int startPos = 0);
	static std::string FormatTag(const std::string& tagString, const std::string& tagContent);

	static std::map<Key, int> m_KeysToVKCodesMap; // map Key enum values to vk codes

	static bool keysdown[sf::Keyboard::KeyCount];
	static bool keysdownLastFrame[sf::Keyboard::KeyCount];
};
