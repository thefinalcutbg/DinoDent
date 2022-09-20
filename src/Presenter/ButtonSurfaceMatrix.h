#pragma once
#include <array>
#include <string>
#include <unordered_map>

#include "InputEnums.h"

class ButtonSurfaceMatrix
{
	std::array<int, 6> upRightPost{ 2,3,1,4,0,5 };
	std::array<int, 6> upRightFront{ 2,5,1,0,3,4 };
	std::array<int, 6> upLeftFront{ 1,5,2,0,3,4 };
	std::array<int, 6> upLeftPost{ 1,3,2,4,0,5 };
	std::array<int, 6> lowLeftPost{ 1,4,2,3,0,5 };
	std::array<int, 6> lowLeftFront{ 1,0,2,5,3,4 };
	std::array<int, 6> lowRightFront{ 2,0,1,5,3,4 };
	std::array<int, 6> lowRightPost{ 2,4,1,3,0,5 };

	std::unordered_map<int, std::array<int, 6>> map;

public:
	ButtonSurfaceMatrix();

	int getSurface(int toothIndex, ButtonPos position);
};

