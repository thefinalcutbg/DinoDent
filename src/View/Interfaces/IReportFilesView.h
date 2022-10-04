#pragma once
#include <vector>
#include "Model/Dental/ReportFile.h"


class IReportFilesView
{
public:

	virtual void setFiles(const std::vector<ReportFile>& fileRows) = 0;
};