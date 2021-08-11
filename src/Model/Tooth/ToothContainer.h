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

	void formatBridges(const std::vector<int>& selectedIndexes);
	void removeBridge(int tooth_idx);

	auto begin() { return teeth->begin(); }
	auto end() { return teeth->end(); }
	auto begin() const { return teeth->begin(); }
	auto end() const { return teeth->end(); };

	constexpr int size() const { return teethNum; }


	Tooth& at(int index) { return (*this)[index]; }
	const Tooth& at(int index) const { return(*this)[index]; }

	ToothContainer& operator=(const ToothContainer& other);
	const Tooth& operator [](int tooth) const;
	Tooth& operator [](int index);

	~ToothContainer();


};

