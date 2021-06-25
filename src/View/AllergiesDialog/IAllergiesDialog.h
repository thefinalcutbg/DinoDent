#pragma once
#include <string>

class AllergiesDialogPresenter;
class Allergies;

class IAllergiesDialog
{
public:
	virtual void setData(const Allergies &allergies) = 0;
	virtual Allergies getData() = 0;
	virtual void close() = 0;
};