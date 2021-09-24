#include "AmbListPage.h"


AmbListPage::AmbListPage(QWidget* parent) :
    QWidget(parent)
{
    ui.setupUi(this);


    connect(ui.newButton, &QPushButton::clicked, [&] { presenter.newAmbPressed(); });
    connect(ui.saveAsButton, &QPushButton::clicked, [&] { presenter.saveAs(); });
    connect(ui.saveButton, &QPushButton::clicked, [&] { presenter.save(); });
    connect(ui.tabView, &TabView::closeRequested, [&] {presenter.closeTab(); });
    connect(ui.listSelectButton, &QPushButton::clicked, [&] {presenter.showListSelector(); });
    connect(ui.printButton, &QPushButton::pressed, [&] {presenter.printPressed(); });
    connect(ui.perioButton, &QPushButton::pressed, [&] {presenter.newPerioPressed(); });
    presenter.setView(this);
}

AmbListPage::~AmbListPage()
{}

ITabView* AmbListPage::tabView()
{
    return ui.tabView;
}

bool AmbListPage::closeAllTabs()
{
    return presenter.closeAllTabs();
}


