#pragma once
#include <array>
#include "Model/Tooth/Tooth.h"
#include <map>

//this class modifies the status of the tooth that is set to it.
//the functions for obturation and caries surfaces are identical, but I can't combine them(different derived types)

class ToothController
{
protected:
	Tooth* tooth;

public:

	ToothController() : tooth(nullptr) {};

    virtual void addStatus(int statusIndex) = 0;
    virtual void removeStatus(int statusIndex) = 0;
	virtual void removeStatus() = 0;
	virtual void setTooth(Tooth* tooth) 
	{
		this->tooth = tooth;
	};
    
};
