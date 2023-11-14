#pragma once
#include <array>
#include <string>
#include <unordered_map>
#include "Model/Dental/Enums.h"
#include "InputEnums.h"

class ButtonSurfaceMatrix
{
	std::array<int, 10> firstQuadrant{
		Surface::Distal,
		Surface::Buccal,
		Surface::Medial,
		Surface::Lingual,
		Surface::Occlusal,
		Surface::Distal,
		Surface::Cervical,
		Surface::Medial,
		Surface::Occlusal,
		Surface::Buccal
	};

	std::array<int, 10> secondQuadrant{
		Surface::Medial,
		Surface::Buccal,
		Surface::Distal,
		Surface::Lingual,
		Surface::Occlusal,
		Surface::Medial,
		Surface::Cervical,
		Surface::Distal,
		Surface::Occlusal,
		Surface::Buccal
	};

	std::array<int, 10> thirdQuadrant{
		Surface::Medial,
		Surface::Lingual,
		Surface::Distal,
		Surface::Buccal,
		Surface::Occlusal,
		Surface::Medial,
		Surface::Occlusal,
		Surface::Distal,
		Surface::Cervical,
		Surface::Buccal
	};

	std::array<int, 10> fourthQuadrant{
		Surface::Distal,
		Surface::Lingual,
		Surface::Medial,
		Surface::Buccal,
		Surface::Occlusal,
		Surface::Distal,
		Surface::Occlusal,
		Surface::Medial,
		Surface::Cervical,
		Surface::Buccal
	};

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

