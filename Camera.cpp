
#include "Camera.h"
#include "Player.h"
#include "Level.h"

const float Camera::DEFAULT_ZOOM = 2.0f;
const float Camera::ACCELERATION = 4.0f;

Camera::Camera(sf::Vector2f windowSize) :
	ApexWindowListener(),
	m_CurrentZoom(DEFAULT_ZOOM),
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
	
	m_ShakeRadius *= std::max(std::min(1.0f - 4.0f * dt, 0.99f), 0.0f);

	if (abs(m_ShakeRadius.x) > 0.1f || 
		abs(m_ShakeRadius.y) > 0.1f)
	{
		m_ShakeOffset.x = m_ShakeRadius.x;
		m_ShakeOffset.y = m_ShakeRadius.y;
	}

	const float dx = playerPos.x - newView.getCenter().x;
	const float dy = playerPos.y - newView.getCenter().y;

	const float acc = ACCELERATION * dt;
	newView.move(dx * acc, dy * acc);

	BoundsCheck(newView, level);
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
	m_CurrentZoom = zoom;
	const sf::Vector2f prevSize = m_View.getSize();
	m_View.setSize(prevSize.x / zoom, prevSize.y / zoom);
}

void Camera::Jolt(float xScale, float yScale)
{
	m_ShakeRadius.x = xScale;
	m_ShakeRadius.y = yScale;
}

void Camera::Shake(float xScale, float yScale)
{
	m_ShakeRadius.x += xScale;
	m_ShakeRadius.y += yScale;
}

void Camera::OnWindowResize(sf::Vector2u windowSize)
{
	m_View.setSize(windowSize.x / m_CurrentZoom, windowSize.y / m_CurrentZoom);
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
