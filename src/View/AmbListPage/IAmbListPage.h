#pragma once
#include "Presenter/AmbListPagePresenter.h"
#include "Presenter/ListPresenter/ListPresenter.h"
#include <string>
#include <map>

enum class DialogAnswer{Yes, No, Cancel};

class IAmbListPage
{
	
public:
	virtual void newTab(int vecPos, std::string tabName) = 0;
	virtual void focusTab(int vecPos) = 0;
	virtual void removeCurrentTab() = 0;
	virtual int openSaveAsDialog(int defaultNum, std::map<int, bool>) = 0;
	virtual DialogAnswer openSaveDialog(std::string title) = 0;
	virtual void changeTabName(std::string tabName) = 0;
};