
#include "Apex\IO\Keyboard.h"
#include "Apex\ApexMain.h"

#include <fstream>
#include <sstream>

namespace apex
{
	bool Keyboard::s_Keysdown[];
	bool Keyboard::s_KeysdownLastFrame[];
	std::map<size_t, Keyboard::Key> Keyboard::s_KeysToVKCodesMap;
	std::string Keyboard::s_FilePath;
	nlohmann::json Keyboard::s_KeybindingsFileJson;

	using nlohmann::json;

	Keyboard::Keyboard()
	{
	}

	Keyboard::~Keyboard()
	{
	}

	void Keyboard::AddKeybinding(size_t keyIndex, const std::string& keyName, size_t defaultVKCode)
	{
		s_KeysToVKCodesMap[keyIndex].vkCode = FindVKCode(keyName, defaultVKCode);
		s_KeysToVKCodesMap[keyIndex].name = keyName;
	}

	int Keyboard::FindVKCode(const std::string& tagName, size_t defaultKey)
	{
		assert(!s_KeybindingsFileJson.empty());

		if (s_KeybindingsFileJson.find(tagName) != s_KeybindingsFileJson.end())
		{
			int vkCode = s_KeybindingsFileJson[tagName].get<int>();
			return vkCode;
		}

		return defaultKey;
	}

	void Keyboard::LoadKeybindingsFromFile()
	{
		if (s_FilePath.empty())
		{
			PrintString("Keybindings file path not set! Call apex::Keyboard::SetKeybindingsFilePath on initialization!", LogType::LOG_ERROR);
			return;
		}

		std::ifstream fileInStream;
		std::stringstream stringStream;

		fileInStream.open(s_FilePath);

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

	void Keyboard::StoreStringData(const std::string& fileContents)
	{
		json file;
		if (!fileContents.empty())
		{
			file = json::parse(fileContents);
			s_KeybindingsFileJson = file["keybindings"];
		}
	}

	void Keyboard::ResetDefaultValues()
	{
		StoreStringData("");
	}

	void Keyboard::SaveKeybindingsToFile()
	{
		std::ofstream fileOutStream;
		std::stringstream stringStream;

		fileOutStream.open(s_FilePath);

		if (fileOutStream)
		{
			json fileJson;
			
			std::map<size_t, Key>::iterator iter;
			for (iter = s_KeysToVKCodesMap.begin(); iter != s_KeysToVKCodesMap.end(); ++iter)
			{
				fileJson["keybindings"][iter->second.name] = iter->second.vkCode;
			}

			fileOutStream << std::setw(4) << fileJson;
			fileOutStream.close();
		}
	}

	void Keyboard::Tick()
	{
		for (size_t i = 0; i < sf::Keyboard::KeyCount; ++i)
		{
			s_KeysdownLastFrame[i] = s_Keysdown[i];
			s_Keysdown[i] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key(i));
		}
	}

	void Keyboard::Clear()
	{
		for (size_t i = 0; i < sf::Keyboard::KeyCount; ++i)
		{
			s_KeysdownLastFrame[i] = false;
			s_Keysdown[i] = false;
		}
	}

	bool Keyboard::IsKeyDown(size_t keyIndex)
	{
		if (s_KeysToVKCodesMap.find(keyIndex) != s_KeysToVKCodesMap.end())
		{
			return s_Keysdown[s_KeysToVKCodesMap[keyIndex].vkCode];
		}
		return false;
	}

	bool Keyboard::IsKeyPressed(size_t keyIndex)
	{
		if (s_KeysToVKCodesMap.find(keyIndex) != s_KeysToVKCodesMap.end())
		{
			const int vkCode = s_KeysToVKCodesMap[keyIndex].vkCode;
			return s_Keysdown[vkCode] && s_KeysdownLastFrame[vkCode] == false;
		}
		return false;
	}

	bool Keyboard::IsKeyReleased(size_t keyIndex)
	{
		if (s_KeysToVKCodesMap.find(keyIndex) != s_KeysToVKCodesMap.end())
		{
			const int vkCode = s_KeysToVKCodesMap[keyIndex].vkCode;
			return s_Keysdown[vkCode] == false && s_KeysdownLastFrame[vkCode];
		}
		return false;
	}

	bool Keyboard::GetKeyFromVKCode(int vkCode, int& keyIndex)
	{
		std::map<size_t, Key>::iterator iter;
		for (iter = s_KeysToVKCodesMap.begin(); iter != s_KeysToVKCodesMap.end(); ++iter)
		{
			if (iter->second.vkCode == vkCode)
			{
				keyIndex = iter->first;
				return true;
			}
		}
		return false;
	}

	bool Keyboard::GetVKCodeFromKey(size_t keyIndex, int& vkCode)
	{
		if (s_KeysToVKCodesMap.find(keyIndex) != s_KeysToVKCodesMap.end())
		{
			vkCode = s_KeysToVKCodesMap[keyIndex].vkCode;
			return true;
		}
		return false;
	}

	void Keyboard::MapKey(size_t keyIndex, int vkCode)
	{
		s_KeysToVKCodesMap[keyIndex].vkCode = vkCode;

		SaveKeybindingsToFile();
	}

	void Keyboard::SetKeybindingsFilePath(const std::string& filePath)
	{
		s_FilePath = filePath;
	}

	size_t Keyboard::GetNumberOfKeys()
	{
		return s_KeysToVKCodesMap.size();
	}

	std::string Keyboard::GetSFKeyName(sf::Keyboard::Key key)
	{
		switch (key)
		{
		case sf::Keyboard::A:
			return "A";
		case sf::Keyboard::B:
			return "B";
		case sf::Keyboard::C:
			return "C";
		case sf::Keyboard::D:
			return "D";
		case sf::Keyboard::E:
			return "E";
		case sf::Keyboard::F:
			return "F";
		case sf::Keyboard::G:
			return "G";
		case sf::Keyboard::H:
			return "H";
		case sf::Keyboard::I:
			return "I";
		case sf::Keyboard::J:
			return "J";
		case sf::Keyboard::K:
			return "K";
		case sf::Keyboard::L:
			return "L";
		case sf::Keyboard::M:
			return "M";
		case sf::Keyboard::N:
			return "N";
		case sf::Keyboard::O:
			return "O";
		case sf::Keyboard::P:
			return "P";
		case sf::Keyboard::Q:
			return "Q";
		case sf::Keyboard::R:
			return "R";
		case sf::Keyboard::S:
			return "S";
		case sf::Keyboard::T:
			return "T";
		case sf::Keyboard::U:
			return "U";
		case sf::Keyboard::V:
			return "V";
		case sf::Keyboard::W:
			return "W";
		case sf::Keyboard::X:
			return "X";
		case sf::Keyboard::Y:
			return "Y";
		case sf::Keyboard::Z:
			return "Z";
		case sf::Keyboard::Num0:
			return "Num0";
		case sf::Keyboard::Num1:
			return "Num1";
		case sf::Keyboard::Num2:
			return "Num2";
		case sf::Keyboard::Num3:
			return "Num3";
		case sf::Keyboard::Num4:
			return "Num4";
		case sf::Keyboard::Num5:
			return "Num5";
		case sf::Keyboard::Num6:
			return "Num6";
		case sf::Keyboard::Num7:
			return "Num7";
		case sf::Keyboard::Num8:
			return "Num8";
		case sf::Keyboard::Num9:
			return "Num9";
		case sf::Keyboard::Escape:
			return "Esc";
		case sf::Keyboard::LControl:
			return "LCtrl";
		case sf::Keyboard::LShift:
			return "LShift";
		case sf::Keyboard::LAlt:
			return "LAlt";
		case sf::Keyboard::LSystem:
			return "LSystem";
		case sf::Keyboard::RControl:
			return "RCtrl";
		case sf::Keyboard::RShift:
			return "RShift";
		case sf::Keyboard::RAlt:
			return "RAlt";
		case sf::Keyboard::RSystem:
			return "RSystem";
		case sf::Keyboard::Menu:
			return "Menu";
		case sf::Keyboard::LBracket:
			return "LBracket";
		case sf::Keyboard::RBracket:
			return "RBracket";
		case sf::Keyboard::SemiColon:
			return "SemiColon";
		case sf::Keyboard::Comma:
			return "Comma";
		case sf::Keyboard::Period:
			return "Period";
		case sf::Keyboard::Quote:
			return "Quote";
		case sf::Keyboard::Slash:
			return "Slash";
		case sf::Keyboard::BackSlash:
			return "BackSlash";
		case sf::Keyboard::Tilde:
			return "Tilde";
		case sf::Keyboard::Equal:
			return "Equal";
		case sf::Keyboard::Dash:
			return "Dash";
		case sf::Keyboard::Space:
			return "Space";
		case sf::Keyboard::Return:
			return "Return";
		case sf::Keyboard::BackSpace:
			return "BackSpace";
		case sf::Keyboard::Tab:
			return "Tab";
		case sf::Keyboard::PageUp:
			return "PageUp";
		case sf::Keyboard::PageDown:
			return "PageDown";
		case sf::Keyboard::End:
			return "End";
		case sf::Keyboard::Home:
			return "Home";
		case sf::Keyboard::Insert:
			return "Insert";
		case sf::Keyboard::Delete:
			return "Delete";
		case sf::Keyboard::Add:
			return "Add";
		case sf::Keyboard::Subtract:
			return "Subtract";
		case sf::Keyboard::Multiply:
			return "Multiply";
		case sf::Keyboard::Divide:
			return "Divide";
		case sf::Keyboard::Left:
			return "Left";
		case sf::Keyboard::Right:
			return "Right";
		case sf::Keyboard::Up:
			return "Up";
		case sf::Keyboard::Down:
			return "Down";
		case sf::Keyboard::Numpad0:
			return "Numpad0";
		case sf::Keyboard::Numpad1:
			return "Numpad1";
		case sf::Keyboard::Numpad2:
			return "Numpad2";
		case sf::Keyboard::Numpad3:
			return "Numpad3";
		case sf::Keyboard::Numpad4:
			return "Numpad4";
		case sf::Keyboard::Numpad5:
			return "Numpad5";
		case sf::Keyboard::Numpad6:
			return "Numpad6";
		case sf::Keyboard::Numpad7:
			return "Numpad7";
		case sf::Keyboard::Numpad8:
			return "Numpad8";
		case sf::Keyboard::Numpad9:
			return "Numpad9";
		case sf::Keyboard::F1:
			return "F1";
		case sf::Keyboard::F2:
			return "F2";
		case sf::Keyboard::F3:
			return "F3";
		case sf::Keyboard::F4:
			return "F4";
		case sf::Keyboard::F5:
			return "F5";
		case sf::Keyboard::F6:
			return "F6";
		case sf::Keyboard::F7:
			return "F7";
		case sf::Keyboard::F8:
			return "F8";
		case sf::Keyboard::F9:
			return "F9";
		case sf::Keyboard::F10:
			return "F10";
		case sf::Keyboard::F11:
			return "F11";
		case sf::Keyboard::F12:
			return "F12";
		case sf::Keyboard::F13:
			return "F13";
		case sf::Keyboard::F14:
			return "F14";
		case sf::Keyboard::F15:
			return "F15";
		case sf::Keyboard::Pause:
			return "Pause";
		case sf::Keyboard::Unknown:
		default:
			return "Unknown";
		}
	}
} // namespace apex
