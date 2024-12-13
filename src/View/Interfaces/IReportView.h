#pragma once

#include <string>

class IReportView
{
public:
	virtual void appendSheet(long long sheetRowid, const std::string& sheetNumber, const std::string& description) = 0;
	virtual void appendText(const std::string& text) = 0;
	virtual void clearText() = 0;
	virtual void setPercent(int percent) = 0;
	virtual void enableReportButtons(bool enabled) = 0;
	virtual void showStopButton(bool yes) = 0;
	virtual void closeDialog() = 0;
};