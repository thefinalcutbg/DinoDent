#pragma once

enum class ButtonPos { 
	o_left, 
	o_up, 
	o_right, 
	o_down, 
	o_center,
	b_left,
	b_up,
	b_right,
	b_down,
	b_center
};

constexpr int otherInputsCount = 8;

namespace OtherInputs
{ 
enum OtherInputs
{
	MO,
	DO,
	MOD,
	removeO,
	removeC,
	removeAll,
	removeBridge,
	removeDsn
};
 
}