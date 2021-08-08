#pragma once
#include <array>
#include <vector>
#include "Model/Tooth/Tooth.h"

class ToothContainer
{
	static constexpr int teethNum = 32;

	std::array<Tooth, teethNum>* teeth;

	void formatSelection(const std::vector<int>& selection);

public:
	ToothContainer();
	ToothContainer(const ToothContainer& other);
	ToothContainer(ToothContainer&& other) noexcept;

	void formatBridges(const std::vector<int>& indexes);
	void removeBridge(int tooth_idx);


	~ToothContainer();

	Tooth& operator [](int tooth);
};

