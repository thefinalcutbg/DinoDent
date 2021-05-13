#pragma once

#include <QWidget>
#include "ui_CrownView.h"

#include "ICrownView.h"

#include "Presenter/ProcedureDialog/SubPresenters/CrownPresenter.h"

class CrownView : public QWidget, public ICrownView
{
	Q_OBJECT

	CrownPresenter* m_presenter;

public:
	CrownView(QWidget *parent = Q_NULLPTR);
	~CrownView();

	// Inherited via ICrownView
	virtual AbstractRangeEdit* rangeWidget() override;
	virtual AbstractLineEdit* materialEdit() override;
	virtual int getType() override;
	virtual std::tuple<int, bool> getColor() override;
	virtual void set_hidden(bool hidden) override;

	void setPresenter(CrownPresenter* presenter);

private:
	Ui::CrownView ui;


};
