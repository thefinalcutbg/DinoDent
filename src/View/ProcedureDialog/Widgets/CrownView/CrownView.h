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
	AbstractRangeEdit* rangeWidget() override;
	AbstractLineEdit* materialEdit() override;
	int getType() override;
	std::tuple<int, bool> getColor() override;
	void set_hidden(bool hidden) override;

	void setData(const BridgeData& data) override;;
	void setData(const CrownData& data) override;;
	virtual CrownData getData() override;
	void setPresenter(CrownPresenter* presenter);

private:
	Ui::CrownView ui;


};
