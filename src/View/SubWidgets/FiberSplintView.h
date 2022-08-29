#pragma once

#include <QWidget>
#include "ui_FiberSplintView.h"

#include "View/Interfaces/IFiberSplintView.h"

#include "Presenter/FiberSplintPresenter.h"

class FiberSplintView : public QWidget,  public IFiberSplintView
{
	Q_OBJECT

	FiberSplintPresenter* m_presenter;

public:
	FiberSplintView(QWidget *parent = Q_NULLPTR);
	~FiberSplintView();

	virtual AbstractRangeEdit* rangeWidget() override;
	virtual void set_hidden(bool hidden) override;
	virtual void setData(const ProcedureFiberData& data) override;
	virtual ObturationData getData() override;

	void setPresenter(FiberSplintPresenter* presenter);

	Ui::FiberSplintView ui;
};
