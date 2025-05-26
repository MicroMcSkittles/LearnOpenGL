#pragma once
#include <string>
#include <functional>

enum class EventType
{
	None = 0,
	WindowClose, WindowResize,
	Error,
	MouseButton
};

// All inharited events must impl a static uint32_t function called "GetStaticType"
class Event {
public:
	virtual uint32_t GetType() const = 0;
	virtual std::string GetName() const = 0;
	virtual std::string ToString() const { return GetName(); }
};

class EventHandler {
public:
	EventHandler(Event& event)
		: m_Event(event) { }

	template <typename T>
	bool Dispatch(std::function<void(T&)> func) {
		if (m_Event.GetType() == T::GetStaticType()) {
			func(*(T*)&m_Event);
			return true;
		}
		return false;
	}

private:
	Event& m_Event;
};

class ErrorEvent : public Event {
public:
	ErrorEvent(const std::string& msg)
		: m_Message(msg) {
	}

	static uint32_t GetStaticType() { return (uint32_t)EventType::Error; }
	virtual uint32_t GetType() const { return (uint32_t)EventType::Error; }
	virtual std::string GetName() const { return "Error Event"; }

	virtual std::string ToString() const { return m_Message; }

private:
	std::string m_Message;
};