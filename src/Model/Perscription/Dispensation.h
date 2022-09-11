#pragma once

struct Dispensation {

	enum Type { SingleUse, MultipleUse, Unlimited };

	Type type{ SingleUse };
	unsigned int repeats{ 2 };//in case of MultipleUse
};