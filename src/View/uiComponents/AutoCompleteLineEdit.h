#pragma once
#include "LineEdit.h"
#include <QCompleter>
#include <QStringListModel>
#include <QAbstractItemView>
#include <unordered_map>
#include <vector>

class AutoCompleteLineEdit : public LineEdit
{

	int maxCharLength;

	void reformat() override;

	void newCompleter(const QStringList& list);

public:

	AutoCompleteLineEdit(QWidget* parent = 0);
	void setCompletions(const std::unordered_map<std::string, int>& map);
	void setCompletions(const std::vector<std::string>& vec);

};


