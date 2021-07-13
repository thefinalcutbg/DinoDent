#include "AmbListPage.h"


AmbListPage::AmbListPage(QWidget* parent) :
    QWidget(parent),
    saveAs_dialog(this)
{
    ui.setupUi(this);


    connect(ui.newButton, &QPushButton::clicked, [=] { presenter.newPressed(); });
    connect(ui.saveAsButton, &QPushButton::clicked, [=] { presenter.saveAs(); });
    connect(ui.saveButton, &QPushButton::clicked, [=] { presenter.save(); });
    connect(ui.tabView, &TabView::closeRequested, [=] {presenter.closeTab(); });
    connect(ui.listSelectButton, &QPushButton::clicked, [=] {presenter.showListSelector(); });

    presenter.setView(this);
}

AmbListPage::~AmbListPage()
{}

ITabView* AmbListPage::tabView()
{
    return ui.tabView;
}

int AmbListPage::openSaveAsDialog(int newNum, std::map<int, bool> existingNumbers)
{
    return saveAs_dialog.exec(existingNumbers, newNum);
}

DialogAnswer AmbListPage::openSaveDialog(std::string title)
{
    SaveDialog saveDialog(QString::fromStdString(title));
    int answer = saveDialog.exec();

    switch (answer)
    {
        case QMessageBox::Yes:
            return DialogAnswer::Yes;
        case QMessageBox::No:
            return DialogAnswer::No;
        default:
            return DialogAnswer::Cancel;
    }
}

bool AmbListPage::closeAllTabs()
{
    return presenter.closeAllTabs();
}


