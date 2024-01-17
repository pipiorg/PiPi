#pragma once

#include <iostream>
#include <map>

using namespace std;

namespace PiPi
{
	class PiPiManageAppearanceException : public runtime_error
	{
	public:
		enum PiPiManageAppearanceExceptionCode
		{
			UnsupportedFieldType = 1
		};

		PiPiManageAppearanceException(PiPiManageAppearanceExceptionCode code);

		const PiPiManageAppearanceExceptionCode getCode();

	private:
		static const map<const PiPiManageAppearanceExceptionCode, const string> PiPiManageAppearanceExceptionMessageCodeMap;

		PiPiManageAppearanceExceptionCode code;
	};
}
