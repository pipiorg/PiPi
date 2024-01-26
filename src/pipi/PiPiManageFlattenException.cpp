#include "PiPiManageFlattenException.h"

namespace PiPi
{
	const map<const PiPiManageFlattenException::PiPiManageFlattenExceptionCode, const string> PiPiManageFlattenException::PiPiManageFlattenExceptionMessageCodeMap{
			{PiPiManageFlattenException::PiPiManageFlattenExceptionCode::CreateFormObjectFromAppearanceFail, "Cannot create object from apperarance"}};

	PiPiManageFlattenException::PiPiManageFlattenException(PiPiManageFlattenException::PiPiManageFlattenExceptionCode code) : runtime_error(PiPiManageFlattenException::PiPiManageFlattenExceptionMessageCodeMap.at(code))
	{
		this->code = code;
	}

	const PiPiManageFlattenException::PiPiManageFlattenExceptionCode PiPiManageFlattenException::getCode()
	{
		return this->code;
	}
}
