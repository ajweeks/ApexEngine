
#include "LightManager.h"
#include "ApexMain.h"
#include "ApexMath.h"
#include "ApexMouse.h"
#include "Level.h"

#include "JSON\json.hpp"

#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\Graphics\CircleShape.hpp>

#include <fstream>
#include <filesystem>

using nlohmann::json;

LightManager::LightManager(int levelIndex, Level* level) :
	ApexMouseListener(),
	m_LevelIndex(levelIndex),
	m_Level(level)
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

	if (m_IsShowingEditor)
	{
		if (ApexMouse::IsButtonDown(sf::Mouse::Left))
		{
			const sf::Vector2f deltaMousePos = APEX->GetMouseCoordsWorldSpace(m_Level->GetCurrentView()) - m_MouseDragStart;
			if (m_CurrentLightDraggingIndex != -1)
			{
				m_Lights[m_CurrentLightDraggingIndex].position = m_PosDragStart + deltaMousePos;
			}
			else if (m_CurrentLightRadiusIndex != -1)
			{
				float radius;
				if (m_MouseDragStart.x < m_Lights[m_CurrentLightRadiusIndex].position.x)
					radius = m_RadiusDragStart - deltaMousePos.x;
				else
					radius = m_RadiusDragStart + deltaMousePos.x;

				const float MIN_RADIUS = 1.0f;
				if (radius < MIN_RADIUS) radius = MIN_RADIUS;
				m_Lights[m_CurrentLightRadiusIndex].radius = radius;
			}
			else if (m_CurrentLightBlurIndex != -1)
			{
				const float X_SCALE = 0.01f;
				float blur;
				if (m_MouseDragStart.x < m_Lights[m_CurrentLightBlurIndex].position.x)
					blur = m_BlurDragStart - deltaMousePos.x * X_SCALE;
				else 
					blur = m_BlurDragStart + deltaMousePos.x * X_SCALE;

				if (blur < 0.0f) blur = 0.0f;
				m_Lights[m_CurrentLightBlurIndex].blur = blur;
				
			}
		}
	}
}

void LightManager::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	// TODO: Bake this render on startup and only update & redraw dynamic lights
	m_LightmapTexture.clear();

	sf::Transform prevTransform = states.transform;

	const sf::Vector2f windowSize = static_cast<sf::Vector2f>(APEX->GetWindowSize());
	sf::RectangleShape screen(windowSize);
	screen.setFillColor(m_AmbientColor);
	m_LightmapTexture.draw(screen);

	screen.setFillColor(sf::Color::Black);
	for (size_t i = 0; i < m_Lights.size(); ++i)
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

	if (m_IsShowingEditor) DrawEditor(target, states);
}

void LightManager::DrawEditor(sf::RenderTarget& target, sf::RenderStates states)
{
	const float crossWidth = 1.0f;
	const float crossLength = 5.0f;

	sf::CircleShape radius;
	radius.setFillColor(sf::Color::Transparent);
	radius.setOutlineThickness(1.0f);
	for (size_t i = 0; i < m_Lights.size(); ++i)
	{
		sf::RectangleShape centerPoint(sf::Vector2f(crossWidth, crossLength));
		centerPoint.setOrigin(crossWidth/2.0f, crossLength / 2.0f);
		centerPoint.setFillColor(sf::Color::White);
		if (m_CurrentLightDraggingIndex == i) 
		{
			centerPoint.setOutlineThickness(0.75f);
		}
		centerPoint.setPosition(m_Lights[i].position);
		target.draw(centerPoint, states);
		centerPoint.rotate(90);
		target.draw(centerPoint, states);

		radius.setOutlineColor(sf::Color::Red);
		radius.setPosition(m_Lights[i].position - sf::Vector2f(m_Lights[i].radius, m_Lights[i].radius));
		radius.setRadius(m_Lights[i].radius);
		target.draw(radius, states);

		radius.setOutlineColor(sf::Color::Green);
		const float blurRadius = m_Lights[i].radius + m_Lights[i].blur * 100.0f;
		radius.setPosition(m_Lights[i].position - sf::Vector2f(blurRadius, blurRadius));
		radius.setRadius(blurRadius);
		target.draw(radius, states);
	}
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

void LightManager::SetShowingEditor(bool showingEditor)
{
	m_IsShowingEditor = showingEditor;
	m_CurrentLightDraggingIndex = -1;
}

void LightManager::ToggleShowingEditor()
{
	SetShowingEditor(!m_IsShowingEditor);
}

bool LightManager::IsShowingEditor() const
{
	return m_IsShowingEditor;
}

bool LightManager::OnButtonPress(sf::Event::MouseButtonEvent buttonEvent)
{
	if (m_IsShowingEditor)
	{
		if (buttonEvent.button == sf::Mouse::Left)
		{
			sf::Vector2f mouseWorldSpace = APEX->GetMouseCoordsWorldSpace(m_Level->GetCurrentView());
				m_MouseDragStart = mouseWorldSpace;
			for (size_t i = 0; i < m_Lights.size(); ++i)
			{
				const float dist = ApexMath::Length(mouseWorldSpace - m_Lights[i].position);
				const float centerSize = m_Lights[i].radius / 2.0f;
				const float radius = m_Lights[i].radius + 3.0f;
				const float blur = m_Lights[i].blur;
				if (dist < centerSize) // Move the light
				{
					m_PosDragStart = m_Lights[i].position;
					m_LastSelectedLightIndex = i;
					m_CurrentLightDraggingIndex = i;
					return false;
				}
				else if (dist < radius) // Change the radius
				{
					m_RadiusDragStart = m_Lights[i].radius;
					m_CurrentLightRadiusIndex = i;
					m_LastSelectedLightIndex = i;
					return false;
				}
				else if (dist < radius + blur * 100.0f + 3.0f) // Change the blur
				{
					m_BlurDragStart = m_Lights[i].blur;
					m_CurrentLightBlurIndex = i;
					m_LastSelectedLightIndex = i;
					return false;
				}
			}
		}
		else if (buttonEvent.button == sf::Mouse::Right)
		{
			if (m_CurrentLightDraggingIndex != -1)
			{
				m_Lights[m_CurrentLightDraggingIndex].position = m_PosDragStart;
				m_CurrentLightDraggingIndex = -1;
			}
			else if (m_CurrentLightRadiusIndex != -1)
			{
				m_Lights[m_CurrentLightRadiusIndex].radius = m_RadiusDragStart;
				m_CurrentLightRadiusIndex = -1;
			}
			else if (m_CurrentLightBlurIndex != -1)
			{
				m_Lights[m_CurrentLightBlurIndex].blur = m_BlurDragStart;
				m_CurrentLightBlurIndex = -1;
			}
		}
	}
	return true;
}

void LightManager::OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent)
{
	m_CurrentLightDraggingIndex = -1;
	m_CurrentLightRadiusIndex = -1;
	m_CurrentLightBlurIndex = -1;
}
