#pragma once

#include <SFML\Graphics.hpp>

#include <JSON\json.hpp>

#include <map>

namespace apex
{
	class Keyboard
	{
	public:
		static void LoadKeybindingsFromFile();
		static void SaveKeybindingsToFile();
		static void ResetDefaultValues();

		static void AddKeybinding(size_t keyIndex, const std::string& keyName, size_t defaultVKCode);

		static void Tick();
		static void Clear();
		static bool IsKeyDown(size_t keyIndex);
		static bool IsKeyPressed(size_t keyIndexkey);
		static bool IsKeyReleased(size_t keyIndex);

		static bool GetKeyFromVKCode(int vkCode, int& keyIndex);
		static bool GetVKCodeFromKey(size_t keyIndex, int& vkCode);

		static void SetKeybindingsFilePath(const std::string& filePath);
		static void MapKey(size_t keyIndex, int vkCode);
		static std::string GetSFKeyName(sf::Keyboard::Key key);
		static size_t GetNumberOfKeys();

		struct Key
		{
			int vkCode;
			std::string name;
		};

	private:
		Keyboard();
		virtual ~Keyboard();

		// TODO: Allow mouse/controller input to replace certain keyboard keys
		static void StoreStringData(const std::string& fileContents);
		static int FindVKCode(const std::string& tagName, size_t defaultKey);

		static std::map<size_t, Key> s_KeysToVKCodesMap; // map key indicies to key objects

		static nlohmann::json s_KeybindingsFileJson;

		static bool s_Keysdown[sf::Keyboard::KeyCount];
		static bool s_KeysdownLastFrame[sf::Keyboard::KeyCount];

		static std::string s_FilePath;
	};
} // namespace apex
