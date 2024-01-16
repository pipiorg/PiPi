#include "PiPiLogCommon.h"

namespace PiPi
{
	void PiPiLogCommon::Init()
	{
		static once_flag log_flag;
		call_once(log_flag, []()
							{
				spdlog::set_level(spdlog::level::warn);
				PoDoFo::PdfCommon::SetMaxLoggingSeverity(PoDoFo::PdfLogSeverity::Debug);
				PoDoFo::PdfCommon::SetLogMessageCallback([](PoDoFo::PdfLogSeverity logSeverity, const string_view& msg) {
					switch (logSeverity) {
					case PoDoFo::PdfLogSeverity::Debug:
						spdlog::debug(msg);
						break;
					case PoDoFo::PdfLogSeverity::Information:
						spdlog::info(msg);
						break;
					case PoDoFo::PdfLogSeverity::Warning:
						spdlog::warn(msg);
						break;
					case PoDoFo::PdfLogSeverity::Error:
						spdlog::error(msg);
						break;
					default:
						break;
					}
					}); });
	}
}
