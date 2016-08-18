
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
bool LightManager::s_LoadedLightingShader = false;

LightManager::LightManager(World* world, const std::string& directory) :
	ApexMouseListener(),
	m_Directory(directory),
	m_World(world)
{
	const sf::Vector2u windowSize = APEX->GetWindowSize();
	m_LightmapTexture.create(windowSize.x, windowSize.y);
	m_LightmapSprite.setTexture(m_LightmapTexture.getTexture());

	LoadLightData();
}

LightManager::~LightManager()
{
}

void LightManager::Tick(sf::Time elapsed)
{
	const float time = APEX->GetTimeElapsed().asSeconds();

	if (m_IsShowingEditor)
	{
		if (ApexMouse::IsButtonDown(sf::Mouse::Left))
		{
			m_NeedUpdate = true;

			const sf::Vector2f deltaMousePos = APEX->GetMouseCoordsWorldSpace(m_World->GetCurrentView()) - m_MouseDragStart;
			if (m_CurrentLightDraggingIndex != -1)
			{
				m_StaticLights[m_CurrentLightDraggingIndex].position = m_PosDragStart + deltaMousePos;
			}
			else if (m_CurrentLightRadiusIndex != -1)
			{
				float radius;
				if (m_MouseDragStart.x < m_StaticLights[m_CurrentLightRadiusIndex].position.x)
					radius = m_RadiusDragStart - deltaMousePos.x;
				else
					radius = m_RadiusDragStart + deltaMousePos.x;

				const float MIN_RADIUS = 1.0f;
				if (radius < MIN_RADIUS) radius = MIN_RADIUS;
				m_StaticLights[m_CurrentLightRadiusIndex].radius = radius;
			}
			else if (m_CurrentLightBlurIndex != -1)
			{
				const float X_SCALE = 0.01f;
				float blur;
				if (m_MouseDragStart.x < m_StaticLights[m_CurrentLightBlurIndex].position.x)
					blur = m_BlurDragStart - deltaMousePos.x * X_SCALE;
				else 
					blur = m_BlurDragStart + deltaMousePos.x * X_SCALE;

				if (blur < 0.0f) blur = 0.0f;
				m_StaticLights[m_CurrentLightBlurIndex].blur = blur;
				
			}
		}
		else if (ApexMouse::IsButtonDown(sf::Mouse::Middle) ||
				 ApexMouse::IsButtonDown(sf::Mouse::Right))
		{
			m_NeedUpdate = true;
		}
	}
}

void LightManager::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
	// TODO: Bake this render on startup and only update & redraw dynamic lights (?)
	if (m_NeedUpdate)
	{
		m_NeedUpdate = false;
		sf::Transform prevTransform = states.transform;

		m_LightmapTexture.clear();

		const sf::Vector2f windowSize = static_cast<sf::Vector2f>(APEX->GetWindowSize());
		sf::RectangleShape screen(windowSize);
		screen.setFillColor(m_AmbientColor);
		m_LightmapTexture.draw(screen);

		screen.setFillColor(sf::Color::Black);
		for (size_t i = 0; i < m_StaticLights.size(); ++i)
		{
			s_LightingShader.setParameter("u_blur", m_StaticLights[i].blur);
			s_LightingShader.setParameter("u_color", m_StaticLights[i].color);
			s_LightingShader.setParameter("u_opacity", m_StaticLights[i].opacity);
			s_LightingShader.setParameter("u_position", m_StaticLights[i].position);
			s_LightingShader.setParameter("u_radius", m_StaticLights[i].radius);

			m_LightmapTexture.draw(screen, &s_LightingShader);
		}

		m_LightmapTexture.display();
		target.draw(m_LightmapSprite, sf::BlendMultiply);
	}
	else
	{
		target.draw(m_LightmapSprite, sf::BlendMultiply);
	}

	if (m_IsShowingEditor) DrawEditor(target, states);
}

void LightManager::DrawEditor(sf::RenderTarget& target, sf::RenderStates states)
{
	const float crossWidth = 1.0f;
	const float crossLength = 5.0f;

	sf::CircleShape radius;
	radius.setFillColor(sf::Color::Transparent);
	radius.setOutlineThickness(1.0f);
	for (size_t i = 0; i < m_StaticLights.size(); ++i)
	{
		sf::RectangleShape centerPoint(sf::Vector2f(crossWidth, crossLength));
		centerPoint.setOrigin(crossWidth/2.0f, crossLength / 2.0f);
		centerPoint.setFillColor(sf::Color::White);
		if (m_CurrentLightDraggingIndex == i) 
		{
			centerPoint.setOutlineThickness(0.75f);
		}
		centerPoint.setPosition(m_StaticLights[i].position);
		target.draw(centerPoint, states);
		centerPoint.rotate(90);
		target.draw(centerPoint, states);

		radius.setOutlineColor(sf::Color::Red);
		radius.setPosition(m_StaticLights[i].position - sf::Vector2f(m_StaticLights[i].radius, m_StaticLights[i].radius));
		radius.setRadius(m_StaticLights[i].radius);
		target.draw(radius, states);

		radius.setOutlineColor(sf::Color::Green);
		const float blurRadius = m_StaticLights[i].radius + m_StaticLights[i].blur * 100.0f;
		radius.setPosition(m_StaticLights[i].position - sf::Vector2f(blurRadius, blurRadius));
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
		m_StaticLights.clear();

		std::string line;
		std::stringstream stringStream;
		while (std::getline(fileStream, line))
		{
			stringStream << line;
		}
		const std::string file = stringStream.str();

		json entireFile = json::parse(file);
		m_AmbientColor = ApexMain::StringToColor(entireFile["ambient"].get<std::string>());
		std::vector<json> lights = entireFile["lights"].get<std::vector<json>>();
		for (size_t i = 0; i < lights.size(); i++)
		{
			ApexLight light;

			light.blur = lights[i]["blur"].get<float>();
			light.color = ApexMain::StringToColor(lights[i]["color"].get<std::string>());
			light.flickers = lights[i]["flickers"].get<bool>();
			light.opacity = lights[i]["opacity"].get<float>();
			light.position = ApexMain::StringToVector2f(lights[i]["position"].get<std::string>());
			light.radius = lights[i]["radius"].get<float>();

			m_StaticLights.push_back(light);
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
	lights["ambient"] = ApexMain::ColorToString(m_AmbientColor);
	for (size_t i = 0; i < m_StaticLights.size(); ++i)
	{
		json light;

		light["blur"] = ApexMath::GetFloatPrecision(m_StaticLights[i].blur, 2);
		light["color"] = ApexMain::ColorToString(m_StaticLights[i].color);
		light["flickers"] = m_StaticLights[i].flickers;
		light["opacity"] = ApexMath::GetFloatPrecision(m_StaticLights[i].opacity, 2);
		std::string pos = ApexMain::Vector2fToString(m_StaticLights[i].position);
		light["position"] = ApexMain::Vector2fToString(m_StaticLights[i].position);
		light["radius"] = ApexMath::GetFloatPrecision(m_StaticLights[i].radius, 2);

		lights["lights"].push_back(light);
	}

	fileStream << std::setw(2) << std::setprecision(1) << lights;

	fileStream.close();
}

void LightManager::LoadShader()
{
	if (!s_LoadedLightingShader)
	{
		if (!s_LightingShader.loadFromFile("resources/shaders/lighting.frag", sf::Shader::Fragment))
		{
			ApexOutputDebugString("\n\n\nCould not compile lighting shader\n\n\n\n");
		}
		const sf::Vector2u windowSize = APEX->GetWindowSize();
		s_LightingShader.setParameter("u_resolution", float(windowSize.x), float(windowSize.y));
		s_LoadedLightingShader = true;
	}
}

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
			for (size_t i = 0; i < m_StaticLights.size(); ++i)
			{
				const float dist = ApexMath::Length(mouseWorldSpace - m_StaticLights[i].position);
				const float centerSize = m_StaticLights[i].radius / 2.0f;
				const float radius = m_StaticLights[i].radius + 3.0f;
				const float blur = m_StaticLights[i].blur;
				if (dist < centerSize) // Move the light
				{
					m_PosDragStart = m_StaticLights[i].position;
					m_LastSelectedLightIndex = i;
					m_CurrentLightDraggingIndex = i;
					return false;
				}
				else if (dist < radius) // Change the radius
				{
					m_RadiusDragStart = m_StaticLights[i].radius;
					m_CurrentLightRadiusIndex = i;
					m_LastSelectedLightIndex = i;
					return false;
				}
				else if (dist < radius + blur * 100.0f + 3.0f) // Change the blur
				{
					m_BlurDragStart = m_StaticLights[i].blur;
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
				m_StaticLights[m_CurrentLightDraggingIndex].position = m_PosDragStart;
				m_CurrentLightDraggingIndex = -1;
			}
			else if (m_CurrentLightRadiusIndex != -1)
			{
				m_StaticLights[m_CurrentLightRadiusIndex].radius = m_RadiusDragStart;
				m_CurrentLightRadiusIndex = -1;
			}
			else if (m_CurrentLightBlurIndex != -1)
			{
				m_StaticLights[m_CurrentLightBlurIndex].blur = m_BlurDragStart;
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
				for (size_t i = 0; i < m_StaticLights.size(); ++i)
				{
					if (ApexMath::Length(mousePosWorldSpace - m_StaticLights[i].position) < m_StaticLights[i].radius / 2.0f)
					{
						m_StaticLights.erase(m_StaticLights.begin() + i);
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

					m_StaticLights.push_back(light);
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
