#pragma once
#include <string>

namespace DbTelemetry
{
	struct Data {
		int ambCount{ 0 },
			perioCount{ 0 },
			prescrCount{ 0 },
			invoiceCount{ 0 },
			patientCount{ 0 },
			noticeCount{ 0 }
		;
		
	};

	Data getData(const std::string practiceRzi, const std::string doctorLpk);
}