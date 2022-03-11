#include "TabView.h"
#include <QABstractScrollArea>

TabView::TabView(QWidget *parent)
	: QWidget(parent), tabPresenter(nullptr)
{
	ui.setupUi(this);


    connect(ui.tabBar, &QTabBar::currentChanged,
        [=](int index)
        {
            if (index == -1)
            {
                tabPresenter->setCurrentTab(index);
                return;
            }

            int tabIndex = ui.tabBar->tabData(index).toInt();

            tabPresenter->setCurrentTab(tabIndex);
        });

    connect(ui.tabBar, &QTabBar::tabCloseRequested,
        [=](int index)
        {
            ui.tabBar->setCurrentIndex(index);
            emit closeRequested();
        });

    ui.scrollArea->setAlignment(Qt::AlignHCenter);

    noTabs = new QLabel(this);
    noTabs->setPixmap(QPixmap("dino.png"));
    noTabs->setAlignment(Qt::AlignCenter);
    noTabs->setStyleSheet("background-color:white");

    ui.scrollArea->setObjectName("ScrollArea");
    setStyleSheet("#ScrollArea{background-color : rgb(246, 245, 250);}");


  //  ui.stackedWidget->addWidget(noTabs);
   // ui.stackedWidget->addWidget(&m_listView);
   // ui.stackedWidget->addWidget(&m_perioView);

    showDinosaur();
    
}

TabView::~TabView()
{
}


void TabView::showTabWidget(QWidget* w)
{
    /*
    ui.stackedWidget->setCurrentWidget(w);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    w->adjustSize();
    ui.stackedWidget->adjustSize();
    */
    if (w == ui.scrollArea->widget()) return;
    ui.scrollArea->takeWidget();
    ui.scrollArea->setWidget(w);
}

void TabView::newTab(int tabIndex, std::string tabName)
{
    ui.tabBar->addNewTab(QString::fromStdString(tabName), tabIndex);

  // ui.tabBar->setCurrentIndex(ui.tabBar->count() - 1);

}

void TabView::focusTab(int vecPos)
{
    for (int i = 0; i < ui.tabBar->count(); i++)
    {
        if (ui.tabBar->tabData(i) == vecPos)
            ui.tabBar->setCurrentIndex(i);
    }
}

void TabView::removeCurrentTab()
{
    int index = ui.tabBar->currentIndex();

    ui.tabBar->closeTab(index);
        
}

void TabView::removeTab(int vecPos)
{
    for (int i = 0; i < ui.tabBar->count(); i++)
        if (ui.tabBar->tabData(i) == vecPos)
            ui.tabBar->closeTab(i);
}

void TabView::changeTabName(std::string tabName)
{
    ui.tabBar->setTabText(ui.tabBar->currentIndex(), QString::fromStdString(tabName));
}

void TabView::setTabPresenter(TabPresenter* presenter)
{
    this->tabPresenter = presenter;
}



#include <QScrollBar>

ScrollPos TabView::getScrollPos()
{
    return ScrollPos
    {
        ui.scrollArea->verticalScrollBar()->value(),
        ui.scrollArea->horizontalScrollBar()->value()
    };
}

void TabView::setScrollPos(ScrollPos scrollPos)
{
    ui.scrollArea->verticalScrollBar()->setValue(scrollPos.height);
    ui.scrollArea->horizontalScrollBar()->setValue(scrollPos.width);
}

void TabView::showListView()
{

    showTabWidget(&m_listView);
    m_perioView.setPresenter(nullptr);
    m_summaryView.setPresenter(nullptr);
}

void TabView::showPerioView()
{
    showTabWidget(&m_perioView);
    m_listView.setPresenter(nullptr);
    m_summaryView.setPresenter(nullptr);
}

void TabView::showSummaryView()
{
    showTabWidget(&m_summaryView);
    m_listView.setPresenter(nullptr);
    m_perioView.setPresenter(nullptr);
}

void TabView::showDinosaur()
{
    showTabWidget(noTabs);
    m_listView.setPresenter(nullptr);
    m_perioView.setPresenter(nullptr);
}

IListView* TabView::listView()
{
    return &m_listView;
}

IPerioView* TabView::perioView()
{
    return &m_perioView;
}

IPatientSummaryView* TabView::summaryView()
{
    return &m_summaryView;
}



