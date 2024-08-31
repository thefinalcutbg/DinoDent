#pragma once

#include "View/Interfaces/ITabView.h"
#include "TabInstance.h"

class AmbList;
struct Patient;
struct RowInstance;
struct Recipient;

class TabPresenter
{

	std::unordered_map<int, TabInstance*> m_tabs;

	int m_indexCounter{ -1 };
	int m_currentIndex{ -1 };

	ITabView* view{ nullptr };

	bool newListAlreadyOpened(const Patient& patient);

	void createNewTab(TabInstance* tabInstance, bool setFocus = true);

	std::shared_ptr<Patient> getPatient_ptr(const Patient& patient);

	static TabPresenter s_singleton;

	TabPresenter() {};
public:

	void setView(ITabView* view);

	TabInstance* currentTab();
	void setCurrentTab(int index);

	void refreshPatientTabNames(long long patientRowId);

	void open(const RowInstance& row, bool setFocus = false);
	void openList(const Patient& patient);
	void openPerio(const Patient& patient);
	void openPerscription(const Patient& patient);
	void openInvoice(const std::string& monthNotif);
	void openInvoice(const Recipient& recipient);
	void openInvoice(long long patientRowId, const std::vector<Procedure>& procedures = {});

	bool documentTabOpened(TabType type, long long rowID) const;
	bool patientTabOpened(long long patientRowid) const;

	void closeTabRequested(int tabId);


	//returns false if not all of the tabs are removed(a.k.a. user breaks the operation)
	bool permissionToLogOut();

	static TabPresenter& get() { return s_singleton; }
};

