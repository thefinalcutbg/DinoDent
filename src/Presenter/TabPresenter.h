#pragma once

#include <unordered_map>
#include "View/Interfaces/ITabView.h"
#include "Database/DbAmbList.h"
#include "ListPresenter.h"

class AmbList;
class Patient;
struct RowInstance;


class TabPresenter
{

	std::unordered_map<int, TabInstance*> m_tabs;

	int m_indexCounter;
	int m_currentIndex;


	ITabView* view;

	bool newListExists(const Patient& patient);
//	bool permissionToClose(int tabId);
	bool tabAlreadyOpened(const RowInstance& tab);
	bool monthNotiAlreadyOpened(int monthNotifNum);

	void createNewTab(TabInstance* tabInstance, bool setFocus = true);

	std::shared_ptr<Patient> getPatient_ptr(const Patient& patient);

	//void removeTabInstance(int tabId);
public:
	
	TabPresenter();

	void setView(ITabView* view);

	TabInstance* currentTab();
	void setCurrentTab(int index);

	void refreshPatientTabNames(long long patientRowId);

	void open(const RowInstance& row, bool setFocus = false);
	void openList(const Patient& patient);
	void openPerio(const Patient& patient);
	void openPerscription(const Patient& patient);
	void openInvoice(const std::string& monthNotif);
	void openInvoice(long long patientRowId, const std::vector<Procedure>& procedures = {});

	bool documentTabOpened(TabType type, long long rowID) const;
	bool patientTabOpened(long long patientRowid) const;

	void closeTabRequested(int tabId);


	//returns false if not all of the tabs are removed(a.k.a. user breaks the operation)
	bool permissionToLogOut();
};

