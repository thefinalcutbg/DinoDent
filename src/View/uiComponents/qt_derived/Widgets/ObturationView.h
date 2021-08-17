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

	AbstractSurfaceSelector* surfaceSelector() override;
	void set_hidden(bool hidden) override;
	void setData(const ProcedureObtData& data) override;
	ProcedureObtData getData() override;

	Ui::ObturationView ui;


};
