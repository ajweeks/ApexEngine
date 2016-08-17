
#include "LightManager.h"
#include "ApexMain.h"
#include "ApexMath.h"
#include "ApexMouse.h"
#include "ApexKeyboard.h"

#include "JSON\json.hpp"

#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\Graphics\CircleShape.hpp>

#include <fstream>
#include <filesystem>

using nlohmann::json;

const std::string LightManager::LIGHTMAP_FILENAME = "lights.json";
sf::Shader LightManager::s_LightingShader;

LightManager::LightManager(World* world, const std::string& directory) :
	ApexMouseListener(),
	m_Directory(directory),
	m_World(world)
{
	const sf::Vector2u windowSize = APEX->GetWindowSize();
	m_LightmapTexture.create(windowSize.x, windowSize.y);
	m_LightmapSprite.setTexture(m_LightmapTexture.getTexture());

	LoadLightData();
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
			const sf::Vector2f deltaMousePos = APEX->GetMouseCoordsWorldSpace(m_World->GetCurrentView()) - m_MouseDragStart;
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
	// TODO: Bake this render on startup and only update & redraw dynamic lights (?)

	sf::Transform prevTransform = states.transform;

	const sf::Vector2f windowSize = static_cast<sf::Vector2f>(APEX->GetWindowSize());
	sf::RectangleShape screen(windowSize);
	screen.setFillColor(m_AmbientColor);
	m_LightmapTexture.draw(screen);

	screen.setFillColor(sf::Color::Black);
	for (size_t i = 0; i < m_Lights.size(); ++i)
	{
		s_LightingShader.setParameter("u_blur", m_Lights[i].blur);
		s_LightingShader.setParameter("u_color", m_Lights[i].color);
		s_LightingShader.setParameter("u_opacity", m_Lights[i].opacity);
		s_LightingShader.setParameter("u_position", m_Lights[i].position);
		s_LightingShader.setParameter("u_radius", m_Lights[i].radius);

		m_LightmapTexture.draw(screen, &s_LightingShader);
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
	s_LightingShader.setParameter("u_resolution", float(windowSize.x), float(windowSize.y));
}

void LightManager::LoadLightData()
{
	std::ifstream fileStream;
	if (!std::experimental::filesystem::exists(m_Directory)) {
		ApexOutputDebugString("Directory not found: " + m_Directory + "\n");
		return;
	}

	fileStream.open(m_Directory + LIGHTMAP_FILENAME);
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
		ApexOutputDebugString("Could not open " + m_Directory + LIGHTMAP_FILENAME + "\n");
	}

	fileStream.close();
}

void LightManager::SaveLightData()
{
	if (!std::experimental::filesystem::exists(m_Directory)) {
		std::experimental::filesystem::create_directory(m_Directory);
	}

	const std::string fileName = "lights.json";
	std::ofstream fileStream;
	fileStream.open(m_Directory + fileName);
	if (!fileStream.is_open())
	{
		ApexOutputDebugString("Could not open " + m_Directory + fileName + "\n");
		fileStream.close();
		return;
	}

	json lights;
	lights["ambient"] = ColorToString(m_AmbientColor);
	for (size_t i = 0; i < m_Lights.size(); ++i)
	{
		json light;

		light["blur"] = ApexMath::GetFloatPrecision(m_Lights[i].blur, 2);
		light["color"] = ColorToString(m_Lights[i].color);
		light["flickers"] = m_Lights[i].flickers;
		light["opacity"] = ApexMath::GetFloatPrecision(m_Lights[i].opacity, 2);
		std::string pos = Vector2fToString(m_Lights[i].position);
		light["position"] = Vector2fToString(m_Lights[i].position);
		light["radius"] = ApexMath::GetFloatPrecision(m_Lights[i].radius, 2);

		lights["lights"].push_back(light);
	}

	fileStream << std::setw(2) << std::setprecision(1) << lights;

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

std::string LightManager::Vector2fToString(sf::Vector2f vector)
{
	std::stringstream s;
	
	s << vector.x << ", " << vector.y;

	return s.str();
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

std::string LightManager::ColorToString(sf::Color color)
{
	if (color.a == 255)	return "rgb(" + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b) + ")";
	return "rgba(" + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b) + ", " + std::to_string(color.a) + ")";
}

void LightManager::LoadShader()
{
	if (!s_LightingShader.loadFromFile("resources/shaders/lighting.frag", sf::Shader::Fragment))
	{
		ApexOutputDebugString("\n\n\nCould not compile lighting shader\n\n\n\n");
	}
	const sf::Vector2u windowSize = APEX->GetWindowSize();
	s_LightingShader.setParameter("u_resolution", float(windowSize.x), float(windowSize.y));
}

//void LightManager::SetWorldIndex(int worldIndex)
//{
//	m_WorldIndex = worldIndex;
//	LoadLightData();
//}

void LightManager::SetShowingEditor(bool showingEditor)
{
	m_IsShowingEditor = showingEditor;
	if (!showingEditor) SaveLightData();
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
		switch (buttonEvent.button)
		{
		case  sf::Mouse::Left:
		{
			sf::Vector2f mouseWorldSpace = APEX->GetMouseCoordsWorldSpace(m_World->GetCurrentView());
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
		} break;
		case sf::Mouse::Right:
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
		} break;
		case sf::Mouse::Middle: // Create/destroy lights
		{
			if (m_CurrentLightDraggingIndex== -1 &&
				m_CurrentLightBlurIndex == -1 &&
				m_CurrentLightRadiusIndex == -1)
			{
				sf::Vector2f mousePosWorldSpace = APEX->GetMouseCoordsWorldSpace(m_World->GetCurrentView());

				bool deletedLight = false;
				for (size_t i = 0; i < m_Lights.size(); ++i)
				{
					if (ApexMath::Length(mousePosWorldSpace - m_Lights[i].position) < m_Lights[i].radius / 2.0f)
					{
						m_Lights.erase(m_Lights.begin() + i);
						deletedLight = true;
						break;
					}
				}

				if (!deletedLight)
				{
					ApexLight light;

					light.blur = 0.2f;
					light.color = sf::Color::White;
					light.flickers = false;
					light.opacity = 0.8f;
					light.position = mousePosWorldSpace;
					light.radius = 50.0f;

					m_Lights.push_back(light);
				}
			}
		} break;
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
