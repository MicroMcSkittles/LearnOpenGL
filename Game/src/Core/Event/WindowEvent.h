#pragma once
#include "Event.h"
#include <sstream>

class WindowCloseEvent : public Event {
public:
	WindowCloseEvent() { }

	static uint32_t GetStaticType() { return (uint32_t)EventType::WindowClose; }
	virtual uint32_t GetType() const { return (uint32_t)EventType::WindowClose; }
	virtual std::string GetName() const { return "Window Close Event"; }
};

class WindowResizeEvent : public Event {
public:
	WindowResizeEvent(uint32_t width, uint32_t height)
		:m_Width(width), m_Height(height) { }

	static uint32_t GetStaticType() { return (uint32_t)EventType::WindowResize; }
	virtual uint32_t GetType() const { return (uint32_t)EventType::WindowResize; }
	virtual std::string GetName() const { return "Window Resize Event"; }

	virtual std::string ToString() const {
		std::stringstream ss;
		ss << "Window Resize { " << m_Width << ", " << m_Height << " }";
		return ss.str();
	}

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

private:
	uint32_t m_Width;
	uint32_t m_Height;
};