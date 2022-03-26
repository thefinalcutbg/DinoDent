#pragma once

#include <unordered_map>
#include "View/TabView/ITabView.h"
#include "Database/DbAmbList.h"

#include "../ListPresenter/ListPresenter.h"

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

	bool tabAlreadyOpened(TabType type, const std::string& rowID);
	bool monthNotiAlreadyOpened(int monthNotifNum);

	void openTab(TabInstance* tabInstance);

	std::shared_ptr<Patient> getPatient_ptr(const Patient& patient);

public:
	
	TabPresenter();

	void setView(ITabView* view);

	TabInstance* currentTab();
	void setCurrentTab(int index);

	void openList(const Patient& patient);
	void openPerio(const Patient& patient);
	void openInvoice(const std::string& monthNotifFilePath);
	void openInvoice(const Procedures& procedures, std::shared_ptr<Patient> patient);
	void open(const RowInstance& row);

	void removeTab(TabType type, const std::string& rowID);
	void removePatientTabs(const std::string& patientID);
	void removeCurrentTab();

};

