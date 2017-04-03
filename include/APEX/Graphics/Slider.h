#pragma once

#include "Apex\IO\MouseListener.h"
#include "Apex\Transition\TransformableTransition.h"
#include "Apex\Transition\ColorTransition.h"

#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\System\Time.hpp>
#include <SFML\Graphics\RectangleShape.hpp>

namespace apex
{
	class Slider : public MouseListener
	{
	public:
		Slider(float width, float height, float left, float top, float initalValue = 0.5f);
		virtual ~Slider();

		Slider(const Slider&) = delete;
		Slider& operator=(const Slider&) = delete;

		void Tick(sf::Time elapsed);
		void Draw(sf::RenderTarget& target, sf::RenderStates states);

		void SetValue(float value);
		float GetValue() const;
		bool BeingDragged() const;

		virtual bool OnButtonPress(sf::Event::MouseButtonEvent buttonEvent);
		virtual void OnButtonRelease(sf::Event::MouseButtonEvent buttonEvent);
		virtual void OnScroll(sf::Event::MouseWheelScrollEvent scrollEvent) {};

	private:
		static const float GRIP_MOVE_MS_PER_PIXEL;
		static const sf::Int32 GRIP_COLOR_TRANSITION_MS;
		static const Transition::EaseType GRIP_MOVE_EASE_TYPE;
		// The grip's relative size to the bg rect
		static const float GRIP_REL_WIDTH;
		static const float GRIP_REL_HEIGHT;

		void CreateGripTransformableTransition(float endPosX, float endPosY, bool instant = false);
		void CreateGripTransformableTransition(sf::Vector2f endPos, bool instant = false);

		sf::RectangleShape m_BackgroundRectangle;
		sf::RectangleShape m_GripRectangle;
		TransformableTransition m_GripTransformableTransition;
		ColorTransition m_GripColorTransition;
		sf::Color m_BackgroundColor;
		sf::Color m_BackgroundColorHover;
		sf::Color m_GripColor;
		sf::Color m_GripColorDragged;

		bool m_BeingDragged = false;
		bool m_HoveredOver = false;

	};
} // namespace apex
