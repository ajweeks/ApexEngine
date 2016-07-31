
#include "ApexKeyboard.h"
#include "ApexMain.h"

#include <fstream>
#include <sstream>

bool ApexKeyboard::keysdown[];
bool ApexKeyboard::keysdownLastFrame[];
std::map<ApexKeyboard::Key, int> ApexKeyboard::m_KeysToVKCodesMap;

using nlohmann::json;

std::string ApexKeyboard::names[] = {
	"Left", "Right", "Up", "Down", "Pause", "Interact", "Screenshot", "ToggleDebugOverlay", "ToggleLightEditor", "Restart"
};

ApexKeyboard::ApexKeyboard()
{
}

ApexKeyboard::~ApexKeyboard()
{
}

void ApexKeyboard::LoadKeybindingsFromFile()
{
	const std::string filePath = "resources/keybindings.json";
	std::ifstream fileInStream;
	std::stringstream stringStream;

	fileInStream.open(filePath);

	if (fileInStream)
	{
		std::string line;
		while (fileInStream.eof() == false)
		{
			std::getline(fileInStream, line);
			stringStream << line;
		}
		fileInStream.close();
	}

	StoreStringData(stringStream.str());
}

void ApexKeyboard::StoreStringData(const std::string& fileContents)
{
	json file;
	json keybindings;
	if (!fileContents.empty()) 
	{
		file = json::parse(fileContents);
		keybindings = file["keybindings"];
	}

	m_KeysToVKCodesMap[Key::MOVE_LEFT] = FindVKCode(keybindings, names[MOVE_LEFT], sf::Keyboard::A);
	m_KeysToVKCodesMap[Key::MOVE_RIGHT] = FindVKCode(keybindings, names[MOVE_RIGHT], sf::Keyboard::D);
	m_KeysToVKCodesMap[Key::MOVE_UP] = FindVKCode(keybindings, names[MOVE_UP], sf::Keyboard::W);
	m_KeysToVKCodesMap[Key::MOVE_DOWN] = FindVKCode(keybindings, names[MOVE_DOWN], sf::Keyboard::S);

	m_KeysToVKCodesMap[Key::PAUSE] = FindVKCode(keybindings, names[PAUSE], sf::Keyboard::Escape);
	m_KeysToVKCodesMap[Key::INTERACT] = FindVKCode(keybindings, names[INTERACT], sf::Keyboard::E);
	m_KeysToVKCodesMap[Key::SCREENSHOT] = FindVKCode(keybindings, names[SCREENSHOT], sf::Keyboard::F10);

	m_KeysToVKCodesMap[Key::DEBUG_TOGGLE_DEBUG_OVERLAY] = FindVKCode(keybindings, names[DEBUG_TOGGLE_DEBUG_OVERLAY], sf::Keyboard::F9);
	m_KeysToVKCodesMap[Key::DEBUG_TOGGLE_LIGHT_EDITOR] = FindVKCode(keybindings, names[DEBUG_TOGGLE_LIGHT_EDITOR], sf::Keyboard::F8);
	m_KeysToVKCodesMap[Key::DEBUG_RESTART] = FindVKCode(keybindings, names[DEBUG_RESTART], sf::Keyboard::R);

	SaveKeybindingsToFile();
}

int ApexKeyboard::FindVKCode(const json& keybindings, const std::string& tagName, sf::Keyboard::Key defaultKey)
{
	if (keybindings.find(tagName) != keybindings.end())
	{
		int vkCode = keybindings[tagName].get<int>();
		return vkCode;
	}

	return defaultKey;
}

void ApexKeyboard::SaveKeybindingsToFile()
{
	std::ofstream fileOutStream;
	const std::string filePath = "resources/keybindings.json";
	std::stringstream stringStream;

	fileOutStream.open(filePath);

	if (fileOutStream)
	{
		json fileJson;

		std::map<Key, int>::iterator iter;
		for (iter = m_KeysToVKCodesMap.begin(); iter != m_KeysToVKCodesMap.end(); ++iter)
		{
			fileJson["keybindings"][names[iter->first]] = iter->second;
		}

		fileOutStream << std::setw(4) << fileJson;
		fileOutStream.close();
	}
}

void ApexKeyboard::ResetDefaultValues()
{
	StoreStringData("");
}

std::string ApexKeyboard::GetTagContent(const std::string& totalString, const std::string & tagString, int startPos)
{
	std::string result;

	const std::string openTag = "<" + tagString + ">";
	const std::string closeTag = "</" + tagString + ">";

	int attributeBegin = totalString.find(openTag, startPos);
	if (attributeBegin != -1)
	{
		attributeBegin += openTag.length();
	}
	int attributeEnd = totalString.find(closeTag, attributeBegin);
	if (attributeBegin == std::string::npos || attributeEnd == std::string::npos)
	{
		return "";
	}
	result = totalString.substr(attributeBegin, attributeEnd - attributeBegin);

	return result;
}

std::string ApexKeyboard::FormatTag(const std::string& tagString, const std::string& tagContent)
{
	std::string result;

	result.append("<" + tagString + ">");
	result.append(tagContent);
	result.append("</" + tagString + ">");

	return result;
}

void ApexKeyboard::Tick()
{
	for (size_t i = 0; i < sf::Keyboard::KeyCount; ++i)
	{
		keysdownLastFrame[i] = keysdown[i];
		keysdown[i] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key(i));
	}
}

void ApexKeyboard::Clear()
{
	for (size_t i = 0; i < sf::Keyboard::KeyCount; ++i)
	{
		keysdownLastFrame[i] = false;
		keysdown[i] = false;
	}
}

bool ApexKeyboard::IsKeyDown(Key key)
{
	return keysdown[m_KeysToVKCodesMap[key]];
}

bool ApexKeyboard::IsKeyPressed(Key key)
{
	if (m_KeysToVKCodesMap.find(key) != m_KeysToVKCodesMap.end())
	{
		const int vkCode = m_KeysToVKCodesMap[key];
		return keysdown[vkCode] && keysdownLastFrame[vkCode] == false;
	}
	return false;
}

bool ApexKeyboard::IsKeyReleased(Key key)
{
	const int vkCode = m_KeysToVKCodesMap[key];
	return keysdown[vkCode] == false && keysdownLastFrame[vkCode];
}

bool ApexKeyboard::GetMappedKey(int vkCode, Key& key)
{
	std::map<Key, int>::iterator iter;
	for (iter = m_KeysToVKCodesMap.begin(); iter != m_KeysToVKCodesMap.end(); ++iter)
	{
		if (iter->second == vkCode)
		{
			key = iter->first;
			return true;
		}
	}
	return false;
}
