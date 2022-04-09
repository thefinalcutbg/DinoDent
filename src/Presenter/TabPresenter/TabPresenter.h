#pragma once

#include <unordered_map>
#include "View/TabView/ITabView.h"
#include "Database/DbAmbList.h"
#include "../ListPresenter/ListPresenter.h"
#include <map>
class AmbList;
class Patient;
struct RowInstance;


class TabPresenter
{

	std::unordered_map<int, TabInstance*> m_tabs;

	int _indexCounter;
	int m_currentIndex;


	ITabView* view;

	bool newListExists(const Patient& patient);

	bool tabAlreadyOpened(TabType type, long long rowID);
	bool monthNotiAlreadyOpened(int monthNotifNum);

	void openTab(TabInstance* tabInstance, bool setFocus = true);

	std::shared_ptr<Patient> getPatient_ptr(const Patient& patient);

	void removeTabInstance(int tabId);
public:
	
	TabPresenter();

	void setView(ITabView* view);

	TabInstance* currentTab();
	void setCurrentTab(int index);

	void refreshPatientTabNames(long long patientRowId);
	void openList(const Patient& patient);
	void openPerio(const Patient& patient);
	void openInvoice(const std::string& monthNotifFilePath);
	void openInvoice(const Procedures& procedures, std::shared_ptr<Patient> patient);
	void open(const RowInstance& row, bool setFocus = false);

	bool documentTabOpened(TabType type, long long rowID) const;
	bool patientTabOpened(long long patientRowid) const;

	void closeTabRequested(int tabId);


	//returns false if not all of the tabs are removed(a.k.a. user breaks the operation)
	bool removeAllTabs();
};

