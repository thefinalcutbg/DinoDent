#pragma once

#include <QWidget>
#include "ui_PriceListSettings.h"
#include "View/Interfaces/IPriceListSettings.h"
#include "Presenter/PriceListSettingsPresenter.h"
#include "View/Models/ProcedureTemplateModel.h"

class PriceListSettings : public QWidget, public IPriceListSettings
{

	PriceListSettingsPresenter* presenter{nullptr};

	ProcedureTemplateModel m_model;

	Q_OBJECT

public:
	PriceListSettings(QWidget *parent = Q_NULLPTR);
	void setPresenter(PriceListSettingsPresenter* presenter) override;
	void setTemplates(const std::vector<ProcedureTemplate>& templates) override;
	void setCurrentIndex(int idx) override;

	~PriceListSettings();

private:
	Ui::PriceListSettings ui;
};
