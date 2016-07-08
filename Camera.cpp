
#include "Camera.h"
#include "Player.h"
#include "Level.h"

const float Camera::DEFAULT_ZOOM = 2.0f;
const float Camera::ACCELERATION = 4.0f;

Camera::Camera(sf::Vector2f windowSize) :
	m_View(sf::FloatRect(0, 0, windowSize.x / DEFAULT_ZOOM, windowSize.y / DEFAULT_ZOOM))
{
}

Camera& Camera::operator=(const Camera& other)
{
	if (&other == this) return *this;

	m_View = other.GetCurrentView();
	return *this;
}

Camera::~Camera()
{
}

void Camera::Tick(sf::Time elapsed, Level* level)
{
	const float dt = elapsed.asSeconds();
	
	Player* player = level->GetPlayer();
	sf::Vector2f playerPos = player->GetPosition();

	sf::View newView(m_View);
	
	m_ShakeRadius *= 1.0f - 0.995f * dt;
	if (m_ShakeRadius.x > 1.0f || 
		m_ShakeRadius.y > 1.0f)
	{
		const float angle = float(std::rand() % 360);
		m_ShakeOffset.x = cos(angle) * m_ShakeRadius.x;
		m_ShakeOffset.y = sin(angle) * m_ShakeRadius.y;
	}

	const float dx = playerPos.x - newView.getCenter().x;
	const float dy = playerPos.y - newView.getCenter().y;


	const float acc = ACCELERATION * dt;
	newView.move(dx * acc, dy * acc);

	//BoundsCheck(newView, level);
	m_View = newView;
}

sf::View Camera::GetCurrentView() const
{
	sf::View view(m_View);
	view.move(m_ShakeOffset);
	return view;
}

void Camera::SetZoom(float zoom)
{
	m_View.setSize(m_View.getSize().x / zoom, m_View.getSize().y / zoom);
}

void Camera::Shake(float xScale, float yScale)
{
	m_ShakeRadius.x = (std::rand() % 1001 / 1000.0f) * xScale;
	m_ShakeRadius.y = (std::rand() % 1001 / 1000.0f) * yScale;
}

void Camera::BoundsCheck(sf::View& view, Level* level)
{
	const float levelWidth = float(level->GetWidth());
	const float levelHeight = float(level->GetHeight());

	const float halfCameraWidth = view.getSize().x / 2.0f;
	const float halfCameraHeight = view.getSize().y / 2.0f;

	if (view.getCenter().x - halfCameraWidth < 0.0f)
	{
		view.setCenter(halfCameraWidth, view.getCenter().y);
	}
	if (view.getCenter().x + halfCameraWidth > levelWidth)
	{
		view.setCenter(levelWidth - halfCameraWidth, view.getCenter().y);
	}

	if (view.getCenter().y - halfCameraHeight < 0.0f)
	{
		view.setCenter(view.getCenter().x, halfCameraHeight);
	}
	if (view.getCenter().y + halfCameraHeight > levelHeight)
	{
		view.setCenter(view.getCenter().x, levelHeight - halfCameraHeight);
	}
}
