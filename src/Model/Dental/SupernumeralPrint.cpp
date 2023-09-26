#include "SupernumeralPrint.h"
#include "ToothContainer.h"

SupernumeralPrint::SupernumeralPrint(Quadrant q, const ToothContainer& teeth)
{
	for (int i = 0; i < 8; i++)
	{
		int idx = static_cast<int>(q)*8 + i;

		if (!teeth[idx].dsn) continue;

		printStatus = teeth[idx].dsn.tooth().getPrintStatus();

		if (q == Quadrant::First || q == Quadrant::Third) {
			position = 8 - i;
		}
		else {
			position = i + 1;
		}
	}
}
