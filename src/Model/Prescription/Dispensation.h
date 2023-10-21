#pragma once

struct Dispensation {

	enum Type { SingleUse, MultipleUse, Unlimited };

	Type type{ SingleUse };
	int repeats{ 2 };//in case of MultipleUse

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

	void setTypeFromNhis(int nhisType) {

		if (nhisType == 1) {
			type = SingleUse;
			repeats = 2;
			return;
		}

		type = MultipleUse;

	}

	void setRepeatsFromNhis(int nhisRepeats) {

		if (!nhisRepeats) {
			type = Unlimited;
			return;
		}

		repeats = nhisRepeats;
		type = MultipleUse;
	}
};