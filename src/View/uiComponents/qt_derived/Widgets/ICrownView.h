#pragma once

struct ProcedureBridgeData;
struct CrownData;
#include "View/uiComponents/qt_derived/Widgets/AbstractRangeEdit.h"

class ICrownView
{
public:
	virtual AbstractRangeEdit* rangeWidget() = 0;
	virtual void set_hidden(bool hidden) = 0;
	virtual void setData(const ProcedureBridgeData& data) = 0;
	virtual void setData(const CrownData& data) = 0;
	virtual CrownData getData() = 0;

};