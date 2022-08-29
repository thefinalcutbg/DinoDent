#pragma once
#include "AbstractSubPresenter.h"

class EditorPresenter : public AbstractSubPresenter
{
	int m_tooth{ -1 };
	int m_temp{ false };
	Date m_date;

public:

	EditorPresenter(const Procedure& p);

	void setCommonFieldsView(ICommonFields* view) override;

	//always returns one procedure
	virtual std::vector<Procedure> getProcedures();


};

