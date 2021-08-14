#pragma once

class CheckModel;

class IDetailsView
{
public:
	virtual void setCheckModel(const CheckModel& model) = 0;
};