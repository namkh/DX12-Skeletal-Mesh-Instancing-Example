#pragma once

#include <windows.h>

enum class ECoreEventType
{
	CORE_EVENT_INVALID_EVENT = -1,
	CORE_EVENT_SYSTEM_EVENT,
	CORE_EVENT_SCREEN_SIZE_CHANGED_EVENT,
	CORE_EVENT_KEYBOARD_EVENT,
	CORE_EVENT_MOUSE_EVENT,
	CORE_EVENT_END_INDEX,
};

enum class EMouseEvent
{
	INVALID_MOUSE_EVENT = 0,
	LBUTTON_DOWN = 1,
	RBUTTON_DOWN = 1 << 1,
	MBUTTON_DOWN = 1 << 2,
	LBUTTON_UP = 1 << 3,
	RBUTTON_UP = 1 << 4,
	MBUTTON_UP = 1 << 5,
	MBUTTON_MOVE = 1 << 6,
	LBUTTON_PRESSED = 1 << 7,
	RBUTTON_PRESSED = 1 << 8,
	MBUTTON_PRESSED = 1 << 9,
	MOUSE_MOVE = 1 << 10,
	MOUSE_WHEEL_UP = 1 << 11,
	MOUSE_WHEEL_DOWN = 1 << 12,
};


enum class EKeyEvent
{
	INVALID_MOUSE_EVENT = 0,
	LBUTTON_DOWN = 1,
	RBUTTON_DOWN = 1 << 1,
	MBUTTON_DOWN = 1 << 2,
	LBUTTON_UP = 1 << 3,
	RBUTTON_UP = 1 << 4,
	MBUTTON_UP = 1 << 5,
	MBUTTON_MOVE = 1 << 6,
	LBUTTON_PRESSED = 1 << 7,
	RBUTTON_PRESSED = 1 << 8,
	MBUTTON_PRESSED = 1 << 9,
	MOUSE_MOVE = 1 << 10,
};

enum class EKeyboaradEvent
{
	INVALID = -1,
	SPACE = VK_SPACE
};

struct EventBase
{
public:
	EventBase()
		: m_eventType(ECoreEventType::CORE_EVENT_INVALID_EVENT)
	{
	
	}
	EventBase(ECoreEventType eventType)
		: m_eventType(eventType)
	{
	
	}
	
	ECoreEventType GetEventType() { return m_eventType; }
public:
	ECoreEventType m_eventType;
};

struct MouseEvent : public EventBase
{
	typedef uint64_t MouseEventField;

	MouseEvent()
		: EventBase(ECoreEventType::CORE_EVENT_MOUSE_EVENT)
	{
	}

	bool CheckMouseEvent(EMouseEvent checkEvent)
	{
		return m_keyEvent & static_cast<MouseEventField>(checkEvent);
	}

	void SetMouseEvent(MouseEventField mouseEventField)
	{
		m_keyEvent = mouseEventField;
	}

	void AddEvent(EMouseEvent mouseEvent)
	{
		m_keyEvent |= static_cast<MouseEventField>(mouseEvent);
	}

	void RemoveEvent(EMouseEvent mouseEvent)
	{
		m_keyEvent &= !static_cast<MouseEventField>(mouseEvent);
	}

	void SetDelta(int dx, int dy)
	{
		m_dx = dx;
		m_dy = dy;
	}

	void ClearEvents()
	{
		m_keyEvent = 0;
	}

	bool HasMouseEvent()
	{
		return m_keyEvent != 0;
	}

	MouseEventField m_keyEvent = 0;

	int m_dx = 0;
	int m_dy = 0;

	int m_posX = 0;
	int m_posY = 0;
};

struct KeyboardEvent : public EventBase
{
	KeyboardEvent()
		: EventBase(ECoreEventType::CORE_EVENT_KEYBOARD_EVENT)
	{
	}
	
	EKeyboaradEvent m_keyType = EKeyboaradEvent::INVALID;
};

struct CoreSystemEvent : public EventBase
{
	CoreSystemEvent()
		: EventBase(ECoreEventType::CORE_EVENT_SYSTEM_EVENT)
	{

	}

	CoreSystemEvent(ECoreEventType coreEventType)
		: EventBase(coreEventType)
	{

	}
};

struct ScreenSizeChangedEvent : public CoreSystemEvent
{
	ScreenSizeChangedEvent()
		: CoreSystemEvent(ECoreEventType::CORE_EVENT_SCREEN_SIZE_CHANGED_EVENT)
	{
		
	}

	ScreenSizeChangedEvent(uint32_t width, uint32_t height)
		: CoreSystemEvent(ECoreEventType::CORE_EVENT_SCREEN_SIZE_CHANGED_EVENT)
		, Width(width)
		, Height(height)
	{

	}

	uint32_t Width = 0;
	uint32_t Height = 0;
};