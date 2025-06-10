#pragma once
#include "Core/Event.h"
#include <sstream>

namespace Event {

	class MouseMoved : public Event {
	public:
		EVENT_DEF(MouseMoved);
		virtual EventCategory GetCategory() override { return (EventCategory)((uint32_t)EventCategory::Input | (uint32_t)EventCategory::Mouse); }

		MouseMoved(float x, float y)
			:m_X(x), m_Y(y) { }

		virtual std::string ToString() override {
			std::stringstream ss;
			ss << "Mouse Moved Event ( " << m_X << ", " << m_Y << " )";
			return ss.str();
		}

		float GetX() const { return m_X; }
		float GetY() const { return m_Y; }

	private:
		float m_X;
		float m_Y;
	};

	class MouseScroll : public Event {
	public:
		EVENT_DEF(MouseScroll);
		virtual EventCategory GetCategory() override { return (EventCategory)((uint32_t)EventCategory::Input | (uint32_t)EventCategory::Mouse); }

		MouseScroll(float x, float y)
			:m_X(x), m_Y(y) { }

		virtual std::string ToString() override {
			std::stringstream ss;
			ss << "Mouse Scroll Event ( " << m_X << ", " << m_Y << " )";
			return ss.str();
		}

		float GetX() const { return m_X; }
		float GetY() const { return m_Y; }

	private:
		float m_X;
		float m_Y;
	};

	class MouseButton : public Event {
	public:
		EVENT_DEF(MouseButton);
		virtual EventCategory GetCategory() override { return (EventCategory)((uint32_t)EventCategory::Input | (uint32_t)EventCategory::Mouse); }

		MouseButton(uint32_t button, uint32_t action, uint32_t mods)
			:m_Button(button), m_Action(action), m_Mods(mods) { }

		virtual std::string ToString() override {
			std::stringstream ss;
			ss << "Mouse Button Event: Button( " << m_Button << " ), Action( " << m_Action << " ), Mods( " << m_Mods << " )";
			return ss.str();
		}

		uint32_t GetButton() const { return m_Button; }
		uint32_t GetAction() const { return m_Action; }
		uint32_t GetMods()   const { return m_Mods; }

	private:
		uint32_t m_Button;
		uint32_t m_Action;
		uint32_t m_Mods;
	};

	class Keyboard : public Event {
	public:
		EVENT_DEF(Keyboard);
		virtual EventCategory GetCategory() override { return (EventCategory)((uint32_t)EventCategory::Input | (uint32_t)EventCategory::KeyBoard); }

		Keyboard(uint32_t key, uint32_t scancode, uint32_t action, uint32_t mods)
			: m_Key(key), m_Scancode(scancode), m_Action(action), m_Mods(mods) { }

		virtual std::string ToString() override {
			std::stringstream ss;
			ss << "Keyboard Event: Key( " << m_Key << " ), Scancode( " << m_Scancode << " ), Action( " << m_Action << " ), Mods( " << m_Mods << " )";
			return ss.str();
		}

		uint32_t GetKey()      const { return m_Key; }
		uint32_t GetScancode() const { return m_Scancode; }
		uint32_t GetAction()   const { return m_Action; }
		uint32_t GetMods()     const { return m_Mods; }

	private:
		uint32_t m_Key;
		uint32_t m_Scancode;
		uint32_t m_Action;
		uint32_t m_Mods;
	};
}