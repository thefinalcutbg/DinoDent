#pragma once

#include <QWidget>

#include "View/Widgets/ListView.h"
#include "View/Widgets/PerioView.h"
#include "View/Widgets/PatientSummary.h"
#include "View/Widgets/FinancialView.h"
#include "View/Widgets/PrescriptionView.h"
#include "View/uiComponents/PixmapLabel.h"
#include "View/Widgets/WelcomeWidget.h"

#include "View/Interfaces/ITabView.h"
#include "ui_TabView.h"

class TabTitle;

class TabView : public QWidget, public ITabView
{
	Q_OBJECT

	PerioView m_perioView;
	ListView m_listView;
	PatientSummary m_summaryView;
	FinancialView m_financialView;
	PrescriptionView m_prescriptionView;

	WelcomeWidget welcomeScreen{ nullptr };

	void showTabWidget(QWidget* w);

	TabTitle* getTabTitle(int tabId);
	int getTabIndex(int tabId);


public:
	TabView(QWidget *parent = Q_NULLPTR);
	~TabView();

	void requestClose(int tabId);

	// Inherited via ITabView
	void removeAllTabs();
	void newTab(int tabId, const TabName& tabName) override;
	void focusTab(int tabId) override;
	void removeCurrentTab() override;
	//void changeTabName(const TabName& tabName) override;
	void changeTabName(const TabName& tabName, int tabId) override;

	void removeTab(int tabId) override;

	ScrollPos getScrollPos();
	void setScrollPos(ScrollPos scrollPos);

	void showListView() override;
    void showPerioView() override;
	void showSummaryView() override;
	void showFinancialView() override;
	void showPerscriptionView() override;
	void showWelcomeScreen() override;

	IListView* listView() override { return &m_listView; }
	IPerioView* perioView() override { return &m_perioView; }
	IPatientSummaryView* summaryView() override { return &m_summaryView; }
	IFinancialView* financialView() override { return &m_financialView; }
	IPrescriptionView* prescriptionView() override { return &m_prescriptionView; }

signals:
	void closeRequested(int mapIndex);

private:
	Ui::TabView ui;




};
