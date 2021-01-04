#include "CoreEventManager.h"

void CoreEventManager::Destroy()
{
	ClearEvents();
}

CoreEventHandle CoreEventManager::RegisterEventCallback(ECoreEventType eventType, CoreEventCallbackBase* eventCommand)
{
	CoreEventHandle eventHandle;
	eventHandle.m_coreEventType = eventType;
	eventHandle.m_eventIndex = reinterpret_cast<CoreEventIndex>(eventCommand);

	m_eventMap[static_cast<int>(eventType)].insert(std::make_pair(eventHandle.m_eventIndex, eventCommand));

	return eventHandle;
}

void CoreEventManager::UnregisterEventCallback(CoreEventHandle eventHandle)
{
	if (m_eventMap[static_cast<int>(eventHandle.m_coreEventType)].find(eventHandle.m_eventIndex) != m_eventMap[static_cast<int>(eventHandle.m_coreEventType)].end())
	{
		m_eventMap[static_cast<int>(eventHandle.m_coreEventType)].erase(eventHandle.m_eventIndex);
	}
	if (m_eventBuffer.find(eventHandle.m_eventIndex) != m_eventBuffer.end())
	{
		CoreEventCallbackBase* removeEvent = m_eventBuffer[eventHandle.m_eventIndex];
		m_eventBuffer.erase(eventHandle.m_eventIndex);

		if (removeEvent != nullptr)
		{
			delete removeEvent;
			removeEvent = nullptr;
		}
	}
}

void CoreEventManager::ExecEvent(ECoreEventType eventType)
{
	for (auto cur : m_eventMap[static_cast<int>(eventType)])
	{
		if (cur.second != nullptr)
		{
			cur.second->ExecEvent();
		}
	}
}

void CoreEventManager::ExecMouseEvent(MouseEvent* mouseEvent)
{
	for (auto cur : m_eventMap[static_cast<int>(ECoreEventType::CORE_EVENT_MOUSE_EVENT)])
	{
		if (cur.second != nullptr)
		{
			MouseEventCallbackBase* mouseEvnet = dynamic_cast<MouseEventCallbackBase*>(cur.second);
			if (mouseEvnet != nullptr)
			{
				mouseEvnet->ExecEvent(mouseEvent);
			}
		}
	}
}

void CoreEventManager::ExecKeyboardEvent(KeyboardEvent* keyboardEvent)
{
	for (auto cur : m_eventMap[static_cast<int>(ECoreEventType::CORE_EVENT_KEYBOARD_EVENT)])
	{
		if (cur.second != nullptr)
		{
			KeyboardEventCallbackBase* keyEvnet = dynamic_cast<KeyboardEventCallbackBase*>(cur.second);
			if (keyEvnet != nullptr)
			{
				keyEvnet->ExecEvent(keyboardEvent);
			}
		}
	}
}

void CoreEventManager::ExecScreenSizeChangedEvent(ScreenSizeChangedEvent* screenSizeChangedEvent)
{
	for (auto cur : m_eventMap[static_cast<int>(ECoreEventType::CORE_EVENT_SCREEN_SIZE_CHANGED_EVENT)])
	{
		if (cur.second != nullptr)
		{
			CoreSystemEventCallbackBase* screenSizeChangedEvnet = dynamic_cast<CoreSystemEventCallbackBase*>(cur.second);
			if (screenSizeChangedEvnet != nullptr)
			{
				screenSizeChangedEvnet->ExecEvent(screenSizeChangedEvent);
			}
		}
	}
}

void CoreEventManager::ClearEvents()
{
	for (auto cur : m_eventMap)
	{
		cur.clear();
	}

	for (auto cur : m_eventBuffer)
	{
		if (cur.second != nullptr)
		{
			delete cur.second;
			cur.second = nullptr;
		}
	}
	m_eventBuffer.clear();
}