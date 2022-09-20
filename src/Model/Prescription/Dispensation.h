#pragma once

struct Dispensation {

	enum Type { SingleUse, MultipleUse, Unlimited };

	Type type{ SingleUse };
	unsigned int repeats{ 2 };//in case of MultipleUse

	std::string getNhisDispensation() const{
		return type == Type::SingleUse ? "1" : "2";
	}

	std::string getNhisRepeats() const {
		
		switch (type)
		{
		case Unlimited: return "0";
		case MultipleUse: return std::to_string(repeats);
		default: return "";
		}

	}
};