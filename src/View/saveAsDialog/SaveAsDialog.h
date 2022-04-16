#pragma once

#include <QLabel>
#include <map>
#include "ui_SaveAsDialog.h"
#include <unordered_set>


class SaveAsDialog : public QDialog
{

    std::unordered_set <int>& existingNumbers;
    int currentNumber;
    long long m_result{ 0 };


public:
    Q_OBJECT
public:
    SaveAsDialog(std::unordered_set<int>& existingNumbers, long long currentNumber, const std::string& docName, QWidget *parent = Q_NULLPTR);
    void setMaxNumLength(int numberCharLength);
    long long result() { return m_result; }
    ~SaveAsDialog();

private:
    Ui::SaveAsDialog ui;
};
