#pragma once
#include "Event.h"
#include <sstream>

class MouseButtonEvent : public Event {
public:
	MouseButtonEvent(int button, int action)
		: m_Button(button), m_Action(action) { }

	static uint32_t GetStaticType() { return (uint32_t)EventType::MouseButton; }
	virtual uint32_t GetType() const { return (uint32_t)EventType::MouseButton; }
	virtual std::string GetName() const { return "Mouse Button Event"; }

	virtual std::string ToString() const {
		std::stringstream ss;
		ss << "Mouse Button { " << m_Button << ", " << m_Action << " }";
		return ss.str();
	}

	int GetButton() { return m_Button; }
	int GetAction() { return m_Action; }

private:
	int m_Button;
	int m_Action;
};