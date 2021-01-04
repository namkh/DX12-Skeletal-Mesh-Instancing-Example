
#include <stdlib.h>

#include "Utils.h"

const std::string Reporter::ERROR_TYPES[static_cast<uint32_t>(EReportType::REPORT_TYPE_END)] =
{
	"[ERROR]",
	"[WARNING]",
	"[MESSAGE]",
	"[MESSAGE]",
	"[LOG]"
};

void Reporter::Report(EReportType reportType, const char* message, long line, const char* file, const char* function, bool withShutdown)
{
	memset(m_reportMessageBuffer, 0, sizeof(char) * MESSAGE_BUFFER_SIZE);
	if (EReportType::REPORT_TYPE_POPUP_MESSAGE == reportType || EReportType::REPORT_TYPE_MESSAGE == reportType)
	{
		memcpy(m_reportMessageBuffer, message, strlen(message));
		sprintf_s(m_reportMessageBuffer, "%s %s\n", ERROR_TYPES[static_cast<uint32_t>(reportType)].c_str(), message);
	}
	else
	{
		sprintf_s(m_reportMessageBuffer, "%s %s\n[FILE] %s\n[FUNCTION] %s\n[LINE] %d", ERROR_TYPES[static_cast<uint32_t>(reportType)].c_str(), message, file, function, line);
	}

	switch (reportType)
	{
	case EReportType::REPORT_TYPE_ERROR:
		ReportError();
		break;
	case EReportType::REPORT_TYPE_WARN:
		ReportWarning();
		break;
	case EReportType::REPORT_TYPE_MESSAGE:
		ReportMessage();
		break;
	case EReportType::REPORT_TYPE_POPUP_MESSAGE:
		ReportPopupMessage();
		break;
	case EReportType::REPORT_TYPE_LOG:
		ReportLog();
		break;
	default:
		break;
	}

	if (withShutdown)
	{
		Shutdown();
	}
}

void Reporter::ReportError()
{
	ReportToPopup();
}

void Reporter::ReportWarning()
{
	//TODO
}

void Reporter::ReportMessage()
{
	//TODO
}

void Reporter::ReportPopupMessage()
{
	ReportToPopup();
}

void Reporter::ReportLog()
{
	//TODO
}

void Reporter::ReportToPopup()
{
#if KCF_WINDOWS_PLATFORM
	MessageBoxA(0, m_reportMessageBuffer, "Message", MB_OK);
#else
	//TODO
#endif	
}

void Reporter::Shutdown()
{
	exit(0);
}

UID UniqueKeyGenerator::GetUID()
{
	uint64_t count = ++m_counter;

	auto now = std::chrono::system_clock::now().time_since_epoch();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now);
	uint32_t msi = static_cast<uint32_t>(ms.count());
	uint64_t msi64 = static_cast<uint64_t>(msi);

	return ((count << 32) | (msi64));
}