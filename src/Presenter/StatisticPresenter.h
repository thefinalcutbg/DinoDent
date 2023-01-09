#pragma once
#include "View/Interfaces/IStatisticView.h"
#include "TabInstance.h"
class ITabView;

class StatisticPresenter : public TabInstance
{
	IStatisticView* view{ nullptr };

public:
	StatisticPresenter(ITabView* tabView, long long rowId = 0);

	// Inherited via TabInstance
	virtual void setDataToView() override;
	virtual bool isNew() override;
	virtual TabName getTabName() override;
	virtual bool save() override;
	virtual void print() override;
	virtual long long rowID() const override;
};