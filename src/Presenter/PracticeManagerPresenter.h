#pragma once

#include "Model/UserStructs.h"
#include "Model/GlobalSettingsData.h"

class PracticeSelectorView;

class PracticeManagerPresenter
{
	PracticeSelectorView* view{ nullptr };
	std::vector<PracticePair> practices;

	void setDbPathToView();
	void setPracticeListToView();

public:
	PracticeManagerPresenter();
	void setView(PracticeSelectorView* view);
	void addClicked();
	void removeClicked(int idx);
	void dbChangePath();
};
