#pragma once

#include <string>

class IReportDialog
{
public:
	virtual void appendText(const std::string& text) = 0;
	virtual void clearText() = 0;
	virtual void setPercent(int percent) = 0;
	virtual void enableReportButtons(bool enabled) = 0;
};