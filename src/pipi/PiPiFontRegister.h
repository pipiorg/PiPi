#pragma once

#include <podofo/podofo.h>
#include <spdlog/spdlog.h>
#include "PiPiFontManager.h"
#include "PiPiFontRegisterException.h"

using namespace PoDoFo;

namespace PiPi
{
	class PiPiFontRegister
	{
		friend class PiPiOperator;

	public:
		PiPiFontRegister(PiPiFontManager* fontManager);


		bool isOperable();

		void EmbedFonts();
		const std::string RegisterFont(char* fontBytes, size_t fontSize);
	private:
		bool operable;

		PiPiFontManager* fontManager;
	};
}
