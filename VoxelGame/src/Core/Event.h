#pragma once
#include "Core/Core.h"
#include <functional>
#include <string>

namespace Event {

	enum class EventType {
		None = 0,
		WindowResize, WindowClose, WindowError,
		MouseMoved,   MouseScroll, MouseButton,
		Keyboard
	};

	enum EventCategory {
		None = 0,
		Window   = BIT(1),
		Mouse    = BIT(2),
		KeyBoard = BIT(3),
		Input    = BIT(4),
	};

	// Used to make setting up events easier
#define EVENT_DEF(type) static EventType StaticGetType()       { return EventType::type; } \
	                    virtual EventType GetType()   override { return EventType::type; } \
	                    virtual std::string GetName() override { return #type; }

	class Event {
	public:
		virtual EventType     GetType() = 0;
		virtual EventCategory GetCategory() = 0;
		virtual std::string   GetName() = 0;
		virtual std::string   ToString() { return GetName(); }
	};

	class EventHandler {
	public:
		EventHandler(Event& e)
			: m_Event(e) { }

		template <typename T>
		void Dispatch(std::function<void(T&)> func) {
			if (T::StaticGetType() == m_Event.GetType()) {
				func(*(T*)&m_Event);
			}
		}

	private:
		Event& m_Event;
	};
}