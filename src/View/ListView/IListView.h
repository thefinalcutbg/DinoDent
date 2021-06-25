#pragma once

#include "Model/AmbList.h"
#include "Model/Patient.h"

#include "IProcedureView.h"
#include "IStatusView.h"

class ListPresenter;

class IListView : public IStatusView, public IProcedureView
{
public:
	virtual void refresh(AmbList& ambList, Patient &patient) = 0;
	virtual void setPresenter(ListPresenter* presenter) = 0;
};