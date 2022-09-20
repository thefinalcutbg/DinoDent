#pragma once

struct ProcedureBridgeData;
struct CrownData;
#include "View/Interfaces/AbstractRangeEdit.h"

class ICrownView
{
public:
	virtual AbstractRangeEdit* rangeWidget() = 0;
	virtual void set_hidden(bool hidden) = 0;
	//sets the bridge checkbox to true
	virtual void setData(const ProcedureBridgeData& data) = 0;
	//sets the bridge checkbox to false
	virtual void setData(const CrownData& data) = 0;
	virtual void lockBridgeCheckbox(bool asChecked = true) = 0;
	virtual CrownData getData() = 0;
	virtual void setMaterial(const std::string& material) = 0;

};