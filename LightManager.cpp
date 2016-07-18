
#include "LightManager.h"
#include "ApexMain.h"
#include "ApexMath.h"

#include "JSON\json.hpp"

#include <SFML\Graphics\RectangleShape.hpp>

#include <fstream>
#include <filesystem>

using nlohmann::json;

LightManager::LightManager(int levelIndex) :
	m_LevelIndex(levelIndex)
{
	//m_Lights.push_back({ sf::Vector2f(250.0f, 100.0f), sf::Color(240, 240, 180), 30.0f, 25.0f, 1.0f });
	//m_Lights.push_back({ sf::Vector2f(50.0f, 20.0f), sf::Color(140, 240, 200), 30.0f, 5.0f, 0.8f });
	
	const sf::Vector2u windowSize = APEX->GetWindowSize();
	m_LightmapTexture.create(windowSize.x, windowSize.y);
	m_LightmapSprite.setTexture(m_LightmapTexture.getTexture());

	LoadLights();
	LoadShader();
}

LightManager::~LightManager()
{
}

void LightManager::Tick(sf::Time elapsed)
{
	const float time = APEX->GetTimeElapsed().asSeconds();
	for (size_t i = 0; i < m_Lights.size(); i++)
	{
		if (m_Lights[i].flickers)
		{
			m_Lights[i].opacity = (sin(time + i * 0.135f + ApexMath::Random(0.0f, 0.1f)) + 1.0f) / 8.0f + 0.75f;
		}
	}
}

void LightManager::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	m_LightmapTexture.clear();

	sf::Transform prevTransform = states.transform;

	const sf::Vector2f windowSize = static_cast<sf::Vector2f>(APEX->GetWindowSize());
	sf::RectangleShape screen(windowSize);
	screen.setFillColor(m_AmbientColor);
	m_LightmapTexture.draw(screen);

	screen.setFillColor(sf::Color::Black);
	for (size_t i = 0; i < m_Lights.size(); i++)
	{
		m_LightingShader.setParameter("u_blur", m_Lights[i].blur);
		m_LightingShader.setParameter("u_color", m_Lights[i].color);
		m_LightingShader.setParameter("u_opacity", m_Lights[i].opacity);
		m_LightingShader.setParameter("u_position", m_Lights[i].position);
		m_LightingShader.setParameter("u_radius", m_Lights[i].radius);

		m_LightmapTexture.draw(screen, &m_LightingShader);
	}

	m_LightmapTexture.display();
	target.draw(m_LightmapSprite, sf::BlendMultiply);
}

void LightManager::OnWindowResize(sf::Vector2u windowSize)
{
	m_LightingShader.setParameter("u_resolution", float(windowSize.x), float(windowSize.y));
}

void LightManager::LoadLights()
{
	std::ifstream fileStream;

	const std::string directory = "resources/level/" + std::to_string(m_LevelIndex) + "/";
	if (!std::experimental::filesystem::exists(directory)) {
		ApexOutputDebugString("Directory not found: " + directory + "\n");
		return;
	}

	const std::string fileName = "lights.json";
	fileStream.open(directory + fileName);
	if (fileStream.is_open())
	{
		m_Lights.clear();

		std::string line;
		std::stringstream stringStream;
		while (std::getline(fileStream, line))
		{
			stringStream << line;
		}
		const std::string file = stringStream.str();

		json entireFile = json::parse(file);
		m_AmbientColor = StringToColor(entireFile["ambient"].get<std::string>());
		std::vector<json> lights = entireFile["lights"].get<std::vector<json>>();
		for (size_t i = 0; i < lights.size(); i++)
		{
			ApexLight light;

			light.blur = lights[i]["blur"].get<float>();
			light.color = StringToColor(lights[i]["color"].get<std::string>());
			light.flickers = lights[i]["flickers"].get<bool>();
			light.opacity = lights[i]["opacity"].get<float>();
			light.position = StringToVector2f(lights[i]["position"].get<std::string>());
			light.radius = lights[i]["radius"].get<float>();

			m_Lights.push_back(light);
		}
	}
	else
	{
		ApexOutputDebugString("Could not open " + directory + fileName + "\n");
	}

	fileStream.close();
}

sf::Vector2f LightManager::StringToVector2f(std::string string)
{
	sf::Vector2f result;

	int commaIndex = string.find(',');
	if (commaIndex != -1)
	{
		result.x = stof(string.substr(0, commaIndex));
		result.y = stof(string.substr(commaIndex + 1));
	}

	return result;
}

// Accepts strings in form "rgb(255, 255, 255)" and "rgba(255, 255, 255, 255)"
sf::Color LightManager::StringToColor(std::string string)
{
	sf::Color result;

	if (string.substr(0, 4).compare("rgba") == 0)
	{
		std::string colorValue = string.substr(5, string.length() - 5 - 1);
		colorValue.erase(std::remove_if(colorValue.begin(), colorValue.end(), isspace), colorValue.end()); // remove whitespace
		std::vector<std::string> colors = ApexSplit(colorValue, ',');
		result.r = stoi(colors[0]);
		result.g = stoi(colors[1]);
		result.b = stoi(colors[2]);
		result.a = stoi(colors[3]);
	} 
	else if (string.substr(0, 3).compare("rgb") == 0)
	{
		std::string colorValue = string.substr(4, string.length() - 4 - 1);
		colorValue.erase(std::remove_if(colorValue.begin(), colorValue.end(), isspace), colorValue.end()); // remove whitespace
		std::vector<std::string> colors = ApexSplit(colorValue, ',');
		result.r = stoi(colors[0]);
		result.g = stoi(colors[1]);
		result.b = stoi(colors[2]);
		result.a = 255;
	}
	else
	{
		ApexOutputDebugString("Could not convert string to color: " + string + "\n");
	}

	return result;
}

void LightManager::LoadShader()
{
	if (!m_LightingShader.loadFromFile("resources/shaders/lighting.frag", sf::Shader::Fragment))
	{
		ApexOutputDebugString("\n\n\nCould not compile lighting shader\n\n\n\n");
	}
	const sf::Vector2u windowSize = APEX->GetWindowSize();
	m_LightingShader.setParameter("u_resolution", float(windowSize.x), float(windowSize.y));
}

void LightManager::SetLevelIndex(int levelIndex)
{
	m_LevelIndex = levelIndex;
	LoadLights();
}
