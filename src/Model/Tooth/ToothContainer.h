#pragma once
#include <array>
#include "Model/Tooth/Tooth.h"

class ToothContainer
{
	static constexpr int teethNum = 32;

	std::array<Tooth, teethNum>* m_teeth;

public:
	ToothContainer();
	ToothContainer(const ToothContainer& other);
	ToothContainer(ToothContainer&& other) noexcept;
	~ToothContainer();
	Tooth& operator [](int tooth);
};

