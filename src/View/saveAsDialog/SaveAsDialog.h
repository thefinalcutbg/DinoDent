#pragma once

#include <QLabel>
#include <map>
#include "ui_SaveAsDialog.h"
#include <unordered_set>


class SaveAsDialog : public QDialog
{

    std::unordered_set <int>& existingNumbers;
    int currentNumber;

public:
    Q_OBJECT
public:
    SaveAsDialog(std::unordered_set<int>& existingNumbers, int currentNumber, const std::string& docName, QWidget *parent = Q_NULLPTR);
    ~SaveAsDialog();

private:
    Ui::SaveAsDialog ui;
};
