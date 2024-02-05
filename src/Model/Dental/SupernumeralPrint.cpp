#include "SupernumeralPrint.h"
#include "ToothContainer.h"

SupernumeralPrint::SupernumeralPrint(Dental::Quadrant q, const ToothContainer& teeth)
{
	for (int i = 0; i < 8; i++)
	{
		int idx = static_cast<int>(q)*8 + i;

		if (!teeth[idx][Dental::HasSupernumeral]) continue;

		printStatus = teeth[idx].getSupernumeral().getPrintStatus();

		if (q == Dental::Quadrant::First || q == Dental::Quadrant::Third) {
			position = 8 - i;
		}
		else {
			position = i + 1;
		}
	}
}
