#pragma once
#include "Presenter/AmbListPagePresenter.h"
#include "Presenter/ListPresenter/ListPresenter.h"
#include "ITabView.h"
#include <string>
#include <map>

enum class DialogAnswer{Yes, No, Cancel};

class IAmbListPage : public ITabView
{
	
public:

	virtual int openSaveAsDialog(int defaultNum, std::map<int, bool>) = 0;
	virtual DialogAnswer openSaveDialog(std::string title) = 0;
	
};