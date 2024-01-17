#pragma once

#include <iostream>
#include <map>

using namespace std;

namespace PiPi
{
	class PiPiManageFlattenException : public runtime_error
	{
	public:
		enum PiPiManageFlattenExceptionCode
		{
			CreateFormObjectFromAppearanceFail = 1,
		};

		PiPiManageFlattenException(PiPiManageFlattenExceptionCode code);

		const PiPiManageFlattenExceptionCode getCode();

	private:
		static const map<const PiPiManageFlattenExceptionCode, const string> PiPiManageFlattenExceptionMessageCodeMap;

		PiPiManageFlattenExceptionCode code;
	};
}
