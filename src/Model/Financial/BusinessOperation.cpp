#include "BusinessOperation.h"
#include "Model/Procedure/Procedure.h"

BusinessOperation::BusinessOperation(const std::string& code, const std::string& name, double price, int quantity) :
	activity_code(code),
	activity_name(name),
	unit_price(price),
	quantity(quantity),
	value_price(price*quantity)
{
}

BusinessOperation::BusinessOperation(const Procedure& p) :
	activity_code(std::to_string(p.code)),
	activity_name(p.name),
	unit_price(p.price),
	quantity(1),
	value_price(unit_price)
{
}

bool BusinessOperation::operator == (const BusinessOperation& other) const
{
	return (activity_code == other.activity_code) &&
		(activity_name == other.activity_name) &&
		(unit_price == other.unit_price);
}

void BusinessOperation::addQuantity(int q)
{
	quantity += q;
	value_price = unit_price * quantity;
}