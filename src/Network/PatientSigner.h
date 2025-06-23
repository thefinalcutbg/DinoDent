#pragma once

#include <string>
#include <vector>

struct PatientSignature {

	std::string signatureObject;
	std::string signatureCertificate;
	std::string signature;
	std::vector<unsigned char> sigImage;

};

namespace PatientSigner {
	
	PatientSignature signWithWacom(const std::string& what, const std::string& who, const std::string& why);
	PatientSignature signWithSignotec(const std::string& what, const std::string& who);
}