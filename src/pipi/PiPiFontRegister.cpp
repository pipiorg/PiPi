#include "PiPiFontRegister.h"

namespace PiPi {
	PiPiFontRegister::PiPiFontRegister(PiPiFontManager* fontManager)
	{
		this->fontManager = fontManager;
		this->operable = true;
	}

	void PiPiFontRegister::EmbedFonts()
	{
		spdlog::trace("EmbedFonts");

		if (!this->IsOperable())
		{
			throw PiPiFontRegisterException(PiPiFontRegisterException::PiPiFontRegisterExceptionCode::InOperable);
		}

		PiPiFontManager* fontManager = this->fontManager;
		fontManager->EmbedFonts();
	}

	const std::string PiPiFontRegister::RegisterFont(char* fontBytes, size_t fontSize)
	{
		spdlog::trace("RegisterFont");

		if (!this->IsOperable())
		{
			throw PiPiFontRegisterException(PiPiFontRegisterException::PiPiFontRegisterExceptionCode::InOperable);
		}

		PiPiFontManager* fontManager = this->fontManager;
		return fontManager->RegisterFont(fontBytes, fontSize);
	}

	bool PiPiFontRegister::IsOperable()
	{
		spdlog::trace("IsOperable");

		return this->operable;
	}
}
