#pragma once

#include <string>

class ExampleAppBase
{
public:
	ExampleAppBase(std::wstring appName, int width, int height)
		: m_width(width)
		, m_height(height)
		, m_appName(appName)
	{
	}

	virtual ~ExampleAppBase() {}

public:
	virtual bool Initialize() = 0;
	virtual void Update(float timeDelta) = 0;
	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void Destroy() = 0;

public:

	uint32_t GetWidth() { return m_width; }
	uint32_t GetHeight() { return m_height; }

	std::wstring GetAppName() { return m_appName; }

public:
	uint32_t m_width = 0;
	uint32_t m_height = 0;

	std::wstring m_appName = L"DX12 Example";
};
