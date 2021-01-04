#pragma once

#include <fstream>
#include <vector>
#include <math.h>
#include <chrono>
#include <map>
#include <assert.h>

#include "Defaults.h"
#include "Singleton.h"

#define INVALID_INDEX_INT INT_MIN
#define INVALID_COMMAND_HANDLE UINT32_MAX

#define RESOURCE_CONTAINER_INITIAL_SIZE 256

#define MAX_DISPATCH_GROUP_COUNT 65536

#define PI 3.14159265359f

#define Assert(expression, message) assert(expression && message)

#define SET_IDENTITY_FLOAT4X4(mat)	\
mat =								\
{									\
	1.0f, 0.0f, 0.0f, 0.0f,			\
	0.0f, 1.0f, 0.0f, 0.0f,			\
	0.0f, 0.0f, 1.0f, 0.0f,			\
	0.0f, 0.0f, 0.0f, 1.0f,			\
};

#define IDENTITY_FLOAT4X4			\
DirectX::XMFLOAT4X4(				\
{									\
	1.0f, 0.0f, 0.0f, 0.0f,			\
	0.0f, 1.0f, 0.0f, 0.0f,			\
	0.0f, 0.0f, 1.0f, 0.0f,			\
	0.0f, 0.0f, 0.0f, 1.0f,			\
})									\

enum class EReportType
{
	REPORT_TYPE_ERROR = 0,
	REPORT_TYPE_WARN,
	REPORT_TYPE_POPUP_MESSAGE,
	REPORT_TYPE_MESSAGE,
	REPORT_TYPE_LOG,
	REPORT_TYPE_END
};

class Reporter : public TSingleton<Reporter>
{
public:
	Reporter(token) {};
public:
	void Report(EReportType reportType, const char* message, long line, const char* file, const char* function, bool withShutdown = false);

protected:
	void ReportError();
	void ReportWarning();
	void ReportMessage();
	void ReportPopupMessage();
	void ReportLog();

	void ReportToPopup();

	void Shutdown();

private:
	static const uint32_t MESSAGE_BUFFER_SIZE = 4096;
	static const std::string ERROR_TYPES[static_cast<uint32_t>(EReportType::REPORT_TYPE_END)];
	char m_reportMessageBuffer[MESSAGE_BUFFER_SIZE];
};

#define REPORT(reportType, message) Reporter::Instance().Report(reportType, message, __LINE__, __FILE__, __FUNCTION__)
#define REPORT_WITH_SHUTDOWN(reportType, message) Reporter::Instance().Report(reportType, message, __LINE__, __FILE__, __FUNCTION__, true)

class RefCounter
{
public:
	void IncRef() { ++m_refCount; }
	void DecRef()
	{
		if (m_refCount > 0)
		{
			--m_refCount;
		}
	}

	uint32_t GetRefCount() { return m_refCount; }

public:
	uint32_t m_refCount = 0;
};

enum class CmdCheckPointType : uint8_t
{
	BEGIN_RENDER_PASS,
	END_RENDER_PASS,
	DRAW,
	COMPUTE,
	GENERIC
};

struct CmdCheckPointData
{
	CmdCheckPointData(const char* name, CmdCheckPointType type, uint32_t cmdIndex) :
		type(type),
		prev(nullptr)
	{
		sprintf_s(this->name, 2048, "%s\n", name);
		commandBufferIndex = cmdIndex;
	}

	char name[2048];
	CmdCheckPointType type;
	uint32_t commandBufferIndex = 0;
	CmdCheckPointData* prev;
};

typedef uint64_t UID;
class UniqueKeyGenerator : public TSingleton<UniqueKeyGenerator>
{
public:
	UniqueKeyGenerator(token) {};
	UID GetUID();

public:
	uint32_t m_counter = 0;
};

class UniqueIdentifier
{
public:
	UniqueIdentifier()
	{
		m_udi = UniqueKeyGenerator::Instance().GetUID();
	}
	UID GetUID()
	{
		return m_udi;
	}
private:

	UID m_udi = 0;
};

static UINT CalcConstantBufferByteSize(UINT byteSize)
{
	// Constant buffers must be a multiple of the minimum hardware
	// allocation size (usually 256 bytes).  So round up to nearest
	// multiple of 256.  We do this by adding 255 and then masking off
	// the lower 2 bytes which store all bits < 256.
	// Example: Suppose byteSize = 300.
	// (300 + 255) & ~255
	// 555 & ~255
	// 0x022B & ~0x00ff
	// 0x022B & 0xff00
	// 0x0200
	// 512
	return (byteSize + 255) & ~255;
}

template <typename T, typename U>
T DivRoundUp(T value, U divisor)
{
	return (value + divisor - 1) / divisor;
}