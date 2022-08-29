#pragma once

#include <QWidget>
#include "ui_CrownView.h"

#include "View/Interfaces/ICrownView.h"

#include "Presenter/CrownPresenter.h"

class CrownView : public QWidget, public ICrownView
{
	Q_OBJECT

	CrownPresenter* m_presenter;

public:
	CrownView(QWidget *parent = Q_NULLPTR);
	~CrownView();

	// Inherited via ICrownView
	AbstractRangeEdit* rangeWidget() override;

	void set_hidden(bool hidden) override;

	void setData(const ProcedureBridgeData& data) override;
	void setData(const CrownData& data) override;
	void setMaterial(const std::string& material) override;
	virtual CrownData getData() override;
	void lockBridgeCheckbox(bool asChecked) override;
	void setPresenter(CrownPresenter* presenter);


	Ui::CrownView ui;


};
