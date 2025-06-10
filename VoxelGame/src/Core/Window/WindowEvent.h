#pragma once
#include "Core/Event.h"
#include <sstream>

namespace Event {

	class WindowClose : public Event {
	public:
		EVENT_DEF(WindowClose);
		virtual EventCategory GetCategory() override { return EventCategory::Window; }

		WindowClose() { }
	};

	class WindowResize : public Event {
	public:
		EVENT_DEF(WindowResize);
		virtual EventCategory GetCategory() override { return EventCategory::Window; }

		WindowResize(uint32_t width, uint32_t height)
			:m_Width(width), m_Height(height) { }

		virtual std::string ToString() override {
			std::stringstream ss;
			ss << "Window Resize Event ( " << m_Width << ", " << m_Height << " )";
			return ss.str();
		}

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

	private:
		uint32_t m_Width;
		uint32_t m_Height;
	};

}