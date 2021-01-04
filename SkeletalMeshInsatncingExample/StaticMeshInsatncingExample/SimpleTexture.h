#pragma once

#include "Utils.h"
#include "CommandBuffers.h"

class SimpleTexture2D : public RefCounter, public UniqueIdentifier
{
	friend class TextureContainer;
public:
	SimpleTexture2D();

	void Destroy();

	ID3D12Resource* GetTextureResource() { return m_resourceBuffer; }
	std::wstring GetSrcFilePath() { return m_srcFilePath; }

protected:
	bool Load(CommandBufferBase* cmdBuffer, const wchar_t* filePath);
	void Unload();

protected:
	ID3D12Resource* m_resourceBuffer = nullptr;
	ID3D12Resource* m_uploadBuffer = nullptr;

	int m_width = 0;
	int m_height = 0;

	std::wstring m_srcFilePath = L"";
};