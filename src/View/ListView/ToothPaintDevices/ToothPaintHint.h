#pragma once

#include <array>


enum class Flag { none, normal, blue, green, red, dakrRed};


struct ToothPaintHint
{
	int index;
	bool temporary{ false };

	Flag tooth{ Flag::normal };

	std::array<Flag, 6> surface
	{
		Flag::none, Flag::none, Flag::none, Flag::none, Flag::none, Flag::none
	};

	Flag extraction{ Flag::none };
	Flag root{ Flag::none };
	Flag implant{ Flag::none };
	Flag endo{ Flag::none };
	Flag lesion{ Flag::none };
	Flag paro{ Flag::none };
	Flag post{ Flag::none };
	Flag crown{ Flag::none };
	Flag dsn{ Flag::none };

	int toothNum;

	int mobility{ 0 }; //?


};
