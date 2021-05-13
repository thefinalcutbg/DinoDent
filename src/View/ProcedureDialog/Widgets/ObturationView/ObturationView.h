#pragma once

#include <QWidget>
#include "ui_ObturationView.h"
#include "Presenter/ProcedureDialog/SubPresenters/ObturationPresenter.h"
#include "IObturationView.h"

class ObturationView : public QWidget, public IObturationView
{
	Q_OBJECT

	ObturationPresenter* m_presenter;

public:
	ObturationView(QWidget *parent = Q_NULLPTR);
	~ObturationView();

	void setPresenter(ObturationPresenter* presenter);

	// Inherited via IObturationView
	virtual AbstractLineEdit* material() override;
	virtual AbstractSurfaceSelector* surfaceSelector() override;
	virtual std::tuple<int, bool> getColor() override;
	virtual bool hasPost() override;
	virtual void set_hidden(bool hidden) override;
	

private:
	Ui::ObturationView ui;


};
