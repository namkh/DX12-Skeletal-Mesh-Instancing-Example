#pragma once

#include <vector>
#include <map>

#include "Singleton.h"
#include "Events.h"
#include "Commands.h"

typedef unsigned long long CoreEventIndex;

struct CoreEventHandle
{
	ECoreEventType m_coreEventType = ECoreEventType::CORE_EVENT_INVALID_EVENT;
	CoreEventIndex m_eventIndex = 0;
};

class CoreEventCallbackBase
{
public:
	CoreEventCallbackBase()
		: m_eventType(ECoreEventType::CORE_EVENT_INVALID_EVENT)
	{
	}
	CoreEventCallbackBase(ECoreEventType eventType)
		: m_eventType(eventType)
	{
	}

	ECoreEventType GetEventType() { return m_eventType; }
	virtual void ExecEvent() {};
public:
	ECoreEventType m_eventType;
};

class KeyboardEventCallbackBase : public CoreEventCallbackBase
{
public:
	KeyboardEventCallbackBase()
		: CoreEventCallbackBase()
	{
		m_eventType = ECoreEventType::CORE_EVENT_KEYBOARD_EVENT;
	}
	virtual void ExecEvent(KeyboardEvent* keyboardEvent) = 0;
};


template<typename OwnerClass, typename FuncPtr>
class KeyboardEventCallback : public KeyboardEventCallbackBase
{
public:
	KeyboardEventCallback(OwnerClass* owner, FuncPtr funcPtr)
		: KeyboardEventCallbackBase()
	{
		m_keyboardEventCallback.Bind(owner, funcPtr);
	}

	virtual void ExecEvent(KeyboardEvent* keyboardEvent) override
	{
		m_keyboardEventCallback.Exec(keyboardEvent);
	}

	CommandWithOneParam<OwnerClass, KeyboardEvent*> m_keyboardEventCallback;
};

class MouseEventCallbackBase : public CoreEventCallbackBase
{
public:
	MouseEventCallbackBase()
		: CoreEventCallbackBase()
	{
		m_eventType = ECoreEventType::CORE_EVENT_MOUSE_EVENT;
	}
	virtual void ExecEvent(MouseEvent* mouseEvent) = 0;
};

template<typename OwnerClass, typename FuncPtr>
class MouseEventCallback : public MouseEventCallbackBase
{
public:
	MouseEventCallback(OwnerClass* owner, FuncPtr funcPtr)
		: MouseEventCallbackBase()
	{
		m_mouseEventCallback.Bind(owner, funcPtr);
	}

	virtual void ExecEvent(MouseEvent* mouseEvent) override
	{
		m_mouseEventCallback.Exec(mouseEvent);
	}

public:
	CommandWithOneParam<OwnerClass, MouseEvent*> m_mouseEventCallback;
};

class CoreSystemEventCallbackBase : public CoreEventCallbackBase
{
public:
	CoreSystemEventCallbackBase()
		: CoreEventCallbackBase()
	{
		//부모 생성자에 인자로 받자
		m_eventType = ECoreEventType::CORE_EVENT_SYSTEM_EVENT;
	}
	virtual void ExecEvent(CoreSystemEvent* mouseEvent) = 0;
};

template<typename OwnerClass, typename FuncPtr, typename Param0Type>
class ScreenSizeChangedEventCallback : public  CoreSystemEventCallbackBase
{
public:
	ScreenSizeChangedEventCallback(OwnerClass* owner, FuncPtr funcPtr)
		: CoreSystemEventCallbackBase()
	{
		m_eventType = ECoreEventType::CORE_EVENT_SCREEN_SIZE_CHANGED_EVENT;
		m_screenSizeChangedEventCallback.Bind(owner, funcPtr);
	}

	virtual void ExecEvent(CoreSystemEvent* screenSizeChangedEvent) override
	{
		m_screenSizeChangedEventCallback.Exec((ScreenSizeChangedEvent*)screenSizeChangedEvent);
	}

public:
	CommandWithOneParam<OwnerClass, ScreenSizeChangedEvent*> m_screenSizeChangedEventCallback;
};

class CoreEventManager : public TSingleton<CoreEventManager>
{
public:
	CoreEventManager(token) {};

public:

	void Destroy();

	CoreEventHandle RegisterEventCallback(ECoreEventType eventType, CoreEventCallbackBase* eventCommand);
	void UnregisterEventCallback(CoreEventHandle eventHandle);
	
	void ExecEvent(ECoreEventType eventType);
	void ExecMouseEvent(MouseEvent* mouseEvent);
	void ExecKeyboardEvent(KeyboardEvent* keyboardEvent);
	void ExecScreenSizeChangedEvent(ScreenSizeChangedEvent* mouseEvent);

	void ClearEvents();

	template<typename OwnerClass, typename FuncPtr>
	CoreEventHandle RegisterMouseEventCallback(OwnerClass* owner, FuncPtr funcPtr)
	{
		CoreEventCallbackBase* mouseCallback = new MouseEventCallback<OwnerClass, FuncPtr>(owner, funcPtr);
		CoreEventHandle eventHandle = RegisterEventCallback(ECoreEventType::CORE_EVENT_MOUSE_EVENT, mouseCallback);
		m_eventBuffer.insert(std::make_pair(eventHandle.m_eventIndex, mouseCallback));
		return eventHandle;
	}

	template<typename OwnerClass, typename FuncPtr>
	CoreEventHandle RegisterKeyboardEventCallback(OwnerClass* owner, FuncPtr funcPtr)
	{
		CoreEventCallbackBase* keyCallback = new KeyboardEventCallback<OwnerClass, FuncPtr>(owner, funcPtr);
		CoreEventHandle eventHandle = RegisterEventCallback(ECoreEventType::CORE_EVENT_KEYBOARD_EVENT, keyCallback);
		m_eventBuffer.insert(std::make_pair(eventHandle.m_eventIndex, keyCallback));
		return eventHandle;
	}

	//타입에 대해 리팩토링 할 방법 없을지 생각해보자
	template<typename OwnerClass, typename FuncPtr>
	CoreEventHandle RegisterScreenSizeChangedEventCallback(OwnerClass* owner, FuncPtr funcPtr)
	{
		CoreEventCallbackBase* screenSizeChangedEventCallback = new ScreenSizeChangedEventCallback<OwnerClass, FuncPtr, CoreSystemEvent*>(owner, funcPtr);		
		CoreEventHandle eventHandle = RegisterEventCallback(ECoreEventType::CORE_EVENT_SCREEN_SIZE_CHANGED_EVENT, screenSizeChangedEventCallback);
		m_eventBuffer.insert(std::make_pair(eventHandle.m_eventIndex, screenSizeChangedEventCallback));
		return eventHandle;
	}

private:

	std::map<CoreEventIndex, CoreEventCallbackBase*> m_eventMap[static_cast<unsigned int>(ECoreEventType::CORE_EVENT_END_INDEX)];
	std::map<CoreEventIndex, CoreEventCallbackBase*> m_eventBuffer;
};

#define gCoreEventMgr CoreEventManager::Instance()