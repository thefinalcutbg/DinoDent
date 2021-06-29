#pragma once

#include <QWidget>
#include "ui_ImplantView.h"

#include "IImplantView.h"

class ImplantPresenter;

class ImplantView : public QWidget, public IImplantView
{
	
	ImplantPresenter* presenter;

	Q_OBJECT

public:
	ImplantView(QWidget *parent = Q_NULLPTR);
	~ImplantView();

	void setPresenter(ImplantPresenter* presenter);

	// Inherited via IImplantView
	virtual void set_hidden(bool hidden) override;
	AbstractLineEdit* systemEdit() override;
	ImplantData getData() override;
	void setData(ImplantData data) override;

private:
	Ui::ImplantView ui;

};
