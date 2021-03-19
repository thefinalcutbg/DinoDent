#pragma once

#include <vector>
#include <array>
#include "Model/CheckState.h"
#include "Model/Tooth/Tooth.h"

class CheckModelCreator
{
	std::vector <CheckModelReciever*> recievers;

	static std::array<bool, 17> generalStatusBool(const Tooth& tooth);
	static std::array<bool, 6> obturationStatusBool(const Tooth& tooth);
	static std::array<bool, 6> cariesStatusBool(const Tooth& tooth);

	template<int Size>
	std::array<CheckState, Size> getModelArray(std::vector<Tooth*>& teeth, std::array<bool, Size>(*function)(const Tooth& tooth));

public:
	CheckModelCreator();
	void addReciever(CheckModelReciever* reciever);

	void refreshModel(std::vector<Tooth*>& selectedTeeth);
};


