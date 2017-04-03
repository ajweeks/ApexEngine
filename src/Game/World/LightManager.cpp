
#include "Game\World\LightManager.h"
#include "Game\Game.h"
#include "Game\Debug\ImGui.h"

#include "Apex\ApexMain.h"
#include "Apex\ApexMath.h"
#include "Apex\IO\Mouse.h"
#include "Apex\IO\Keyboard.h"

#include <JSON\json.hpp>

#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\Graphics\CircleShape.hpp>
#include <SFML\Graphics\RenderWindow.hpp>

#include <fstream>
#include <filesystem>

using nlohmann::json;

const std::string LightManager::LIGHTMAP_FILENAME = "lights.json";

sf::Shader LightManager::s_LightingShader;
bool LightManager::s_LoadedLightingShader = false;

LightManager::LightManager(World& world, const std::string& directory) :
	apex::MouseListener(),
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
		if (m_SelectedLightIndex != -1)
		{
			if (ImGui::CollapsibleElement("Lights"))
			{
				ImGui::Text("index: " + std::to_string(m_SelectedLightIndex) + "/" + std::to_string(m_StaticLights.size()));
				ImGui::Text("position: " + ImGui::ToString(m_StaticLights[m_SelectedLightIndex].position));
				ImGui::RGBSlider("color: ", m_StaticLights[m_SelectedLightIndex].color);
				ImGui::Text("radius: " + ImGui::ToString(m_StaticLights[m_SelectedLightIndex].radius));
				ImGui::Text("blur: " + ImGui::ToString(m_StaticLights[m_SelectedLightIndex].blur));
				ImGui::Text("flickers: " + ImGui::ToString(m_StaticLights[m_SelectedLightIndex].flickers));
				ImGui::Text("opacity: " + ImGui::ToString(m_StaticLights[m_SelectedLightIndex].opacity));
			}
			ImGui::CollapsibleElementPop();
		}

		if (apex::Mouse::IsButtonDown(sf::Mouse::Left))
		{
			m_NeedUpdate = true;

			const sf::View worldView = m_World.GetCurrentView();
			const sf::Vector2f mouseCoordsWorldSpace = APEX->GetMouseCoordsWorldSpace(m_World.GetCurrentView());
			if (m_CurrentLightDraggingIndex != -1)
			{
				m_StaticLights[m_CurrentLightDraggingIndex].position = mouseCoordsWorldSpace;
			}
			else if (m_CurrentLightRadiusIndex != -1)
			{
				float radius;
				const sf::Vector2f deltaMouseMovement = m_MousePosDragStartWorld - mouseCoordsWorldSpace;
				if (m_MousePosDragStartWorld.x < m_StaticLights[m_CurrentLightRadiusIndex].position.x)
					radius = m_RadiusDragStart + deltaMouseMovement.x;
				else
					radius = m_RadiusDragStart - deltaMouseMovement.x;

				const float MIN_RADIUS = 1.0f;
				if (radius < MIN_RADIUS) radius = MIN_RADIUS;
				m_StaticLights[m_CurrentLightRadiusIndex].radius = radius;
			}
			else if (m_CurrentLightBlurIndex != -1)
			{
				const float X_SCALE = 0.01f;
				float blur;
				const sf::Vector2f deltaMouseMovement = m_MousePosDragStartWorld - mouseCoordsWorldSpace;
				if (m_MousePosDragStartWorld.x < m_StaticLights[m_CurrentLightBlurIndex].position.x)
					blur = m_BlurDragStart + deltaMouseMovement.x * X_SCALE;
				else 
					blur = m_BlurDragStart - deltaMouseMovement.x * X_SCALE;

				if (blur < 0.0f) blur = 0.0f;
				m_StaticLights[m_CurrentLightBlurIndex].blur = blur;
			}
		}
		else if (apex::Mouse::IsButtonDown(sf::Mouse::Middle) ||
				 apex::Mouse::IsButtonDown(sf::Mouse::Right))
		{
			m_NeedUpdate = true;
		}
	}
}

void LightManager::Draw(sf::RenderTarget& target, sf::RenderStates states)
{
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
			s_LightingShader.setUniform("u_blur", m_StaticLights[i].blur);
			s_LightingShader.setUniform("u_color", apex::NormalizeColor(m_StaticLights[i].color));
			s_LightingShader.setUniform("u_opacity", m_StaticLights[i].opacity);
			s_LightingShader.setUniform("u_position", m_StaticLights[i].position);
			s_LightingShader.setUniform("u_radius", m_StaticLights[i].radius);

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
	sf::RectangleShape centerPoint(sf::Vector2f(crossWidth, crossLength));
	centerPoint.setOrigin(crossWidth/2.0f, crossLength / 2.0f);
	for (size_t i = 0; i < m_StaticLights.size(); ++i)
	{
		centerPoint.setFillColor(sf::Color(180, 160, 185));
		radius.setOutlineThickness(1.0f);
		if (m_CurrentLightDraggingIndex == i)
		{
			centerPoint.setOutlineThickness(0.75f);
		}
		if (m_SelectedLightIndex == i)
		{
			centerPoint.setFillColor(sf::Color::White);
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
	s_LightingShader.setUniform("u_resolution", sf::Glsl::Vec2(float(windowSize.x), float(windowSize.y)));
}

void LightManager::LoadLightData()
{
	std::ifstream fileStream;
	if (!std::experimental::filesystem::exists(m_Directory)) {
		apex::PrintString("Directory not found: " + m_Directory + "\n", apex::LogType::LOG_ERROR);
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
		m_AmbientColor = apex::StringToColor(entireFile["ambient"].get<std::string>());
		std::vector<json> lights = entireFile["lights"].get<std::vector<json>>();
		for (size_t i = 0; i < lights.size(); i++)
		{
			ApexLight light;

			light.blur = lights[i]["blur"].get<float>();
			light.color = apex::StringToColor(lights[i]["color"].get<std::string>());
			light.flickers = lights[i]["flickers"].get<bool>();
			light.opacity = lights[i]["opacity"].get<float>();
			light.position = apex::StringToVector2f(lights[i]["position"].get<std::string>());
			light.radius = lights[i]["radius"].get<float>();

			m_StaticLights.push_back(light);
		}

		if (lights.size() > 0) m_SelectedLightIndex = 0;
	}
	else
	{
		apex::PrintString("Could not open " + m_Directory + LIGHTMAP_FILENAME + "\n", apex::LogType::LOG_ERROR);
	}

	fileStream.close();

	s_LoadedLightingShader = false;
	LoadShaders();
	m_NeedUpdate = true;
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
		apex::PrintString("Could not open " + m_Directory + fileName + "\n", apex::LogType::LOG_ERROR);
		fileStream.close();
		return;
	}

	json lights;
	lights["ambient"] = apex::ColorToString(m_AmbientColor);
	for (size_t i = 0; i < m_StaticLights.size(); ++i)
	{
		json light;

		light["blur"] = apex::ApexMath::GetFloatPrecision(m_StaticLights[i].blur, 2);
		light["color"] = apex::ColorToString(m_StaticLights[i].color);
		light["flickers"] = m_StaticLights[i].flickers;
		light["opacity"] = apex::ApexMath::GetFloatPrecision(m_StaticLights[i].opacity, 2);
		std::string pos = apex::Vector2fToString(m_StaticLights[i].position);
		light["position"] = apex::Vector2fToString(m_StaticLights[i].position);
		light["radius"] = apex::ApexMath::GetFloatPrecision(m_StaticLights[i].radius, 2);

		lights["lights"].push_back(light);
	}

	fileStream << std::setw(2) << std::setprecision(1) << lights;

	fileStream.close();
}

void LightManager::LoadShaders()
{
	if (!s_LoadedLightingShader)
	{
		if (!s_LightingShader.loadFromFile("resources/shaders/lighting.frag", sf::Shader::Fragment))
		{
			apex::PrintString("\n\n\nCould not compile lighting shader\n\n\n\n", apex::LogType::LOG_ERROR);
		}
		const sf::Vector2u windowSize = APEX->GetWindowSize();
		s_LightingShader.setUniform("u_resolution", sf::Glsl::Vec2(float(windowSize.x), float(windowSize.y)));
		s_LoadedLightingShader = true;
	}
}

void LightManager::SetShowingEditor(bool showingEditor)
{
	m_IsShowingEditor = showingEditor;
	if (!showingEditor) SaveLightData();
	m_CurrentLightDraggingIndex = -1;
}

bool LightManager::IsShowingEditor() const
{
	return m_IsShowingEditor;
}

bool LightManager::OnButtonPress(sf::Event::MouseButtonEvent buttonEvent)
{
	if (m_IsShowingEditor)
	{
		const sf::Vector2f mousePosWorldSpace = APEX->GetMouseCoordsWorldSpace(m_World.GetCurrentView());
		switch (buttonEvent.button)
		{
		case  sf::Mouse::Left:
		{
			for (size_t i = 0; i < m_StaticLights.size(); ++i)
			{
				const float dist = apex::ApexMath::Length(mousePosWorldSpace - m_StaticLights[i].position);
				const float centerSize = m_StaticLights[i].radius / 2.0f;
				const float radius = m_StaticLights[i].radius + 3.0f;
				const float blur = m_StaticLights[i].blur;
				if (dist < centerSize) // Move the light
				{
					m_StartPosDrag = m_StaticLights[i].position;
					m_CurrentLightDraggingIndex = i;
					m_SelectedLightIndex = i;
					return true;
				}
				else if (dist < radius) // Change the radius
				{
					m_MousePosDragStartWorld = mousePosWorldSpace;
					m_RadiusDragStart = m_StaticLights[i].radius;
					m_CurrentLightRadiusIndex = i;
					m_SelectedLightIndex = i;
					return true;
				}
				else if (dist < radius + blur * 100.0f + 3.0f) // Change the blur
				{
					m_MousePosDragStartWorld = mousePosWorldSpace;
					m_BlurDragStart = m_StaticLights[i].blur;
					m_CurrentLightBlurIndex = i;
					m_SelectedLightIndex = i;
					return true;
				}
			}
		} break;
		case sf::Mouse::Right:
		{
			if (m_CurrentLightDraggingIndex != -1)
			{
				m_StaticLights[m_CurrentLightDraggingIndex].position = m_StartPosDrag;
				m_CurrentLightDraggingIndex = -1;
				return true;
			}
			else if (m_CurrentLightRadiusIndex != -1)
			{
				m_StaticLights[m_CurrentLightRadiusIndex].radius = m_RadiusDragStart;
				m_CurrentLightRadiusIndex = -1;
				return true;
			}
			else if (m_CurrentLightBlurIndex != -1)
			{
				m_StaticLights[m_CurrentLightBlurIndex].blur = m_BlurDragStart;
				m_CurrentLightBlurIndex = -1;
				return true;
			}
		} break;
		case sf::Mouse::Middle: // Create/destroy lights
		{
			if (m_CurrentLightDraggingIndex== -1 &&
				m_CurrentLightBlurIndex == -1 &&
				m_CurrentLightRadiusIndex == -1)
			{
				bool deletedLight = false;
				for (size_t i = 0; i < m_StaticLights.size(); ++i)
				{
					if (apex::ApexMath::Length(mousePosWorldSpace - m_StaticLights[i].position) < m_StaticLights[i].radius / 2.0f)
					{
						m_StaticLights.erase(m_StaticLights.begin() + i);
						if (m_SelectedLightIndex == i) m_SelectedLightIndex = i - 1;
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

					m_SelectedLightIndex = m_StaticLights.size() - 1;
				}
				return true;
			}
		} break;
		}

		m_NeedUpdate = true;
	}
	return false;
}

void LightManager::OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent)
{
	m_CurrentLightDraggingIndex = -1;
	m_CurrentLightRadiusIndex = -1;
	m_CurrentLightBlurIndex = -1;
}
