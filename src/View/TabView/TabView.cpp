#include "TabView.h"
#include <QABstractScrollArea>
#include <QScrollBar>
#include "View/Theme.h"
#include "TabTitle.h"

TabView::TabView(QWidget *parent)
	: QWidget(parent), tabPresenter(nullptr)
{
	ui.setupUi(this);

    ui.tabBar->setExpanding(false);
    ui.tabBar->setMovable(true);
    ui.tabBar->setTabsClosable(true);
    ui.tabBar->setElideMode(Qt::TextElideMode::ElideNone);

    ui.tabBar->setStyleSheet(
        "QTabBar::tab{"
        "background-color:" + Theme::getRGBStringFromColor(Theme::inactiveTabBG) +
        "border-top-left-radius: 8px;"
        "border-top-right-radius: 8px;"
        "margin-right: 1px;"
        "}"

        "QTabBar::tab:selected {"
        "background-color: " + Theme::getRGBStringFromColor(Theme::background) +
        "}"

        "QTabBar::tab:hover:!selected {"
        "background-color:" + Theme::getRGBStringFromColor(Theme::inactiveTabBGHover) +
        "}"
    );




    connect(ui.tabBar, &QTabBar::currentChanged,
        [=](int index)
        {
            if (index == -1)
            {
                tabPresenter->setCurrentTab(index);
                return;
            }

            auto tabId = static_cast<TabTitle*>
                (ui.tabBar->tabButton(index, QTabBar::ButtonPosition::RightSide))
                    ->getTabId();

            tabPresenter->setCurrentTab(tabId);
        });


    ui.scrollArea->setAlignment(Qt::AlignHCenter);
 
    noTabs = new QLabel(this);
    noTabs->setPixmap(QPixmap("dino.png"));
    noTabs->setAlignment(Qt::AlignCenter);
    noTabs->setStyleSheet("background-color:"+ Theme::getRGBStringFromColor(Theme::background));

    ui.scrollArea->setObjectName("ScrollArea");
    setStyleSheet("#ScrollArea{background-color:"+ Theme::getRGBStringFromColor(Theme::background) + "}");


  //  ui.stackedWidget->addWidget(noTabs);
   // ui.stackedWidget->addWidget(&m_listView);
   // ui.stackedWidget->addWidget(&m_perioView);

    showDinosaur();
    
}


void TabView::requestClose(int tabId)
{
    tabPresenter->closeTabRequested(tabId);
}

TabTitle* TabView::getTabTitle(int tabId)
{
    for (int i = 0; i < ui.tabBar->count(); i++)
    {
        auto tab = static_cast<TabTitle*>(ui.tabBar->tabButton(i, QTabBar::ButtonPosition::RightSide));
        
        if (tab->getTabId() == tabId) {
            return tab;
        }
    }

    return nullptr;
}

int TabView::getTabIndex(int tabId)
{
    for (int i = 0; i < ui.tabBar->count(); i++)
    {
        auto tab = static_cast<TabTitle*>(ui.tabBar->tabButton(i, QTabBar::ButtonPosition::RightSide));

        if (tab->getTabId() == tabId) {
            return i;
        }
    }

    return -1;
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

void TabView::newTab(int tabId, const TabName& tabName)
{
    TabTitle* tab = new TabTitle(this, tabId);

    tab->setText(QString::fromStdString(tabName.header), QString::fromStdString(tabName.footer));

    {
        QSignalBlocker блоцкер(ui.tabBar);

        int newTabIdx = ui.tabBar->addTab("");

        ui.tabBar->setTabButton(newTabIdx, QTabBar::ButtonPosition::RightSide, tab);

    }
    //if the tab, which is added is the first one:
    if (ui.tabBar->count() == 1) ui.tabBar->currentChanged(0);
    


}

void TabView::focusTab(int tabId)
{    
    ui.tabBar->setCurrentIndex(getTabIndex(tabId));

}

void TabView::removeCurrentTab()
{
    ui.tabBar->removeTab(ui.tabBar->currentIndex());
}

void TabView::removeTab(int tabId)
{
    ui.tabBar->removeTab(getTabIndex(tabId));
}

void TabView::changeTabName(const TabName& tabName)
{
    auto tab = static_cast<TabTitle*>
        (ui.tabBar->tabButton(ui.tabBar->currentIndex(), QTabBar::ButtonPosition::RightSide));

    tab->setText(QString::fromStdString(tabName.header), QString::fromStdString(tabName.footer));

    //re-setting the tab into the tabbar, to change the layout
    ui.tabBar->setTabButton(ui.tabBar->currentIndex(), QTabBar::ButtonPosition::RightSide, nullptr);
    ui.tabBar->setTabButton(ui.tabBar->currentIndex(), QTabBar::ButtonPosition::RightSide, tab);
}

void TabView::changeTabName(const TabName& tabName, int tabId)
{
    int tabIndex = getTabIndex(tabId);

    auto tab = static_cast<TabTitle*>
        (ui.tabBar->tabButton(tabIndex, QTabBar::ButtonPosition::RightSide));

    tab->setText(QString::fromStdString(tabName.header), QString::fromStdString(tabName.footer));
    
    //re-setting the tab into the tabbar, to change the layout
    ui.tabBar->setTabButton(tabIndex, QTabBar::ButtonPosition::RightSide, nullptr);
    ui.tabBar->setTabButton(tabIndex, QTabBar::ButtonPosition::RightSide, tab);
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
    m_financialView.setPresenter(nullptr);
}

void TabView::showFinancialView()
{
    showTabWidget(&m_financialView);
    m_listView.setPresenter(nullptr);
    m_perioView.setPresenter(nullptr);
    m_summaryView.setPresenter(nullptr);
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

IFinancialView* TabView::financialView()
{
    return &m_financialView;
}


TabView::~TabView()
{
}



