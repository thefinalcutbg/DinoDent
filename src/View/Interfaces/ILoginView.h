#pragma once

#include <vector>
#include <string>

class ILoginView
{
public:
	virtual void setDoctorList(const std::vector < std::string>& doctorList) = 0;
	virtual void disablePasswordField(bool disabled) = 0;
	virtual int practiceUserChoice(const std::vector<std::string>& practiceList) = 0;
	virtual void closeLogin() = 0;
};