#include "PiPiManageAppearanceException.h"

namespace PiPi
{
	const map<const PiPiManageAppearanceException::PiPiManageAppearanceExceptionCode, const string> PiPiManageAppearanceException::PiPiManageAppearanceExceptionMessageCodeMap{
			{PiPiManageAppearanceException::PiPiManageAppearanceExceptionCode::UnsupportedFieldType, "Unsupported field type"} };

	PiPiManageAppearanceException::PiPiManageAppearanceException(PiPiManageAppearanceException::PiPiManageAppearanceExceptionCode code) : runtime_error(PiPiManageAppearanceException::PiPiManageAppearanceExceptionMessageCodeMap.at(code))
	{
		this->code = code;
	}

	const PiPiManageAppearanceException::PiPiManageAppearanceExceptionCode PiPiManageAppearanceException::getCode()
	{
		return this->code;
	}
}
