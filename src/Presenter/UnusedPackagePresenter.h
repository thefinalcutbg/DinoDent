#pragma once
#include "Model/Date.h"

class UnusedPackageView;

class UnusedPackagePresenter
{
	UnusedPackageView* view;

public:
	UnusedPackagePresenter(UnusedPackageView* view);
	void generateReport(const Date& date);
};