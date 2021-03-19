#pragma once
#include <array>

enum class CheckState { unchecked, checked, partially_checked };

struct CheckModel
{
	CheckModel() : 
		generalStatus { CheckState::unchecked },
		obturationStatus{ CheckState::unchecked },
		cariesStatus{ CheckState::unchecked } 
	{};

	CheckModel(
		std::array<CheckState, 17> generalStatus,
		std::array<CheckState, 6> obturationStatus,
		std::array<CheckState, 6> cariesStatus
	) :
		generalStatus(generalStatus),
		obturationStatus(obturationStatus),
		cariesStatus(cariesStatus) 
	{};

	std::array<CheckState, 17> generalStatus;
	std::array<CheckState, 6> obturationStatus;
	std::array<CheckState, 6> cariesStatus;
};

class CheckModelReciever
{
public:
	virtual void setCheckModel(const CheckModel& checkModel) = 0;
};