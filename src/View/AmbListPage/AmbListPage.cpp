#include "AmbListPage.h"



AmbListPage::AmbListPage(QWidget* parent) :
    QWidget(parent),
    saveAs_dialog(this),
    patient_dialog(database, this),
    database(database),
    presenter(this, patient_dialog.Presenter(), list_view.Presenter()),
    previousTabData(0)
{
    ui.setupUi(this);

    list_view.Presenter()->setPatientDialog(patient_dialog.Presenter());

    list_view.Presenter()->attachEditObserver(&presenter);

    QLabel* dummy = new QLabel;
    dummy->setFixedSize(QSize(0, 0));
    ui.scrollArea->setWidget(dummy);


    connect(ui.newButton, &QPushButton::clicked, [=] { presenter.newPressed(); });
    connect(ui.saveAsButton, &QPushButton::clicked, [=] { presenter.saveAs(); });
    connect(ui.saveButton, &QPushButton::clicked, [=] { presenter.save(); });

    connect(ui.tabBar, &QTabBar::currentChanged,
        [=](int index)
        {
            if (index == -1)
            {
                presenter.tabChanged(index);
                return;
            }

            int vecPos = ui.tabBar->tabData(index).toInt();


            ui.scrollArea->saveCurrentPosition();

            //ui.scrollArea->changePosition(vecPos);//bug with scrollbar

            presenter.tabChanged(vecPos);

            ui.scrollArea->changePosition(vecPos); //bug with gl view;


        });

    connect(ui.tabBar, &QTabBar::tabCloseRequested,
        [=](int index)
        {
            ui.tabBar->setCurrentIndex(index);
            presenter.closeTab();
        });

}

AmbListPage::~AmbListPage()
{

}

AmbListPagePresenter* AmbListPage::Presenter()
{
    return &this->presenter;
}

void AmbListPage::newTab(int vecPos, std::string tabName)
{
    if (!ui.tabBar->count())
        ui.scrollArea->setWidget(&list_view);

    ui.scrollArea->addTab();
    ui.tabBar->addNewTab(QString::fromStdString(tabName), vecPos);
    ui.tabBar->setCurrentIndex(ui.tabBar->count() - 1);

}

void AmbListPage::focusTab(int vecPos)
{
    for (int i = 0; i < ui.tabBar->count(); i++)
    {
        if(ui.tabBar->tabData(i) == vecPos)
            ui.tabBar->setCurrentIndex(i);
    }
    ui.scrollArea->tabChanged(vecPos);
}

void AmbListPage::removeCurrentTab()
{
    int index = ui.tabBar->currentIndex();

    ui.scrollArea->removeTab(index);

    ui.tabBar->closeTab(index);

    if (!ui.tabBar->count())
        ui.scrollArea->takeWidget();
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
        case QMessageBox::Cancel:
            return DialogAnswer::Cancel;
    }
}

bool AmbListPage::closeAllTabs()
{
    ui.tabBar->setCurrentIndex(ui.tabBar->count() - 1);

    while (ui.tabBar->count())
    {
        bool success = presenter.closeTab();
        if (!success) return false;
    }
    return true;
}

void AmbListPage::changeTabName(std::string tabName)
{

    ui.tabBar->setTabText(ui.tabBar->currentIndex(), QString::fromStdString(tabName));

}




