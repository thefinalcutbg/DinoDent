#include "EMedicalNotice.h"

bool EMedicalNotice::Issue::sendRequest(const MedicalNotice& notice, const Patient& patient, std::function<void(const std::string&)> nrnCallback)
{
	return false;
}


void EMedicalNotice::Issue::parseReply(const std::string& reply)
{

}