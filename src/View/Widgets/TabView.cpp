#include "TabView.h"
#include <QABstractScrollArea>
#include <QScrollBar>
#include "View/Theme.h"
#include "View/SubWidgets/TabTitle.h"

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
        "background-color:" + Theme::colorToString(Theme::inactiveTabBG) +
        "border-top-left-radius: 8px;"
        "border-top-right-radius: 8px;"
        "margin-right: 1px;"
        "}"

        "QTabBar::tab:selected {"
        "background-color: " + Theme::colorToString(Theme::background) +
        "}"

        "QTabBar::tab:hover:!selected {"
        "background-color:" + Theme::colorToString(Theme::inactiveTabBGHover) +
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
 
   

    ui.scrollArea->setObjectName("ScrollArea");
    setStyleSheet("#ScrollArea{background-color:"+ Theme::colorToString(Theme::background) + "}");

    
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

void TabView::removeAllTabs()
{
    ui.tabBar->blockSignals(true);
    
    while (ui.tabBar->count()) {
        ui.tabBar->removeTab(0);
    }

    ui.tabBar->blockSignals(false);
    
    ui.tabBar->currentChanged(-1);

}

void TabView::newTab(int tabId, const TabName& tabName)
{
    TabTitle* tab = new TabTitle(this, tabId);

    tab->setNhifIcon(tabName.nhif ? QPixmap(":/icons/icon_nhif.png") : QPixmap{});
    tab->setHisIcon(tabName.his ? QPixmap(":/icons/icon_his.png") : QPixmap{});
    tab->setText(QString::fromStdString(tabName.header), QString::fromStdString(tabName.footer));

    {
        QSignalBlocker blocker(ui.tabBar);

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
/*
void TabView::changeTabName(const TabName& tabName)
{
    auto tab = static_cast<TabTitle*>
        (ui.tabBar->tabButton(ui.tabBar->currentIndex(), QTabBar::ButtonPosition::RightSide));

    setTabIcon(ui.tabBar->currentIndex(), tabName.nhif, tabName.his);
    tab->setText(QString::fromStdString(tabName.header), QString::fromStdString(tabName.footer));

    //re-setting the tab into the tabbar, to change the layout
    ui.tabBar->setTabButton(ui.tabBar->currentIndex(), QTabBar::ButtonPosition::RightSide, nullptr);
    ui.tabBar->setTabButton(ui.tabBar->currentIndex(), QTabBar::ButtonPosition::RightSide, tab);
}
*/
void TabView::changeTabName(const TabName& tabName, int tabId)
{
    int tabIndex = getTabIndex(tabId);

    if (tabIndex == -1) return;

    auto tab = static_cast<TabTitle*>
        (ui.tabBar->tabButton(tabIndex, QTabBar::ButtonPosition::RightSide));

    (tabName.nhif) ?
        tab->setNhifIcon(QPixmap(":/icons/icon_nhif.png"))
        :
        tab->setNhifIcon(QPixmap(""));

    (tabName.his) ?
        tab->setHisIcon(QPixmap(":/icons/icon_his.png"))
        :
        tab->setHisIcon(QPixmap(""));

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
    m_prescriptionView.setPresenter(nullptr);
    m_financialView.setPresenter(nullptr);
}

void TabView::showPerioView()
{
    showTabWidget(&m_perioView);
    m_listView.setPresenter(nullptr);
    m_summaryView.setPresenter(nullptr);
    m_prescriptionView.setPresenter(nullptr);
    m_financialView.setPresenter(nullptr);

}

void TabView::showSummaryView()
{
    showTabWidget(&m_summaryView);
    m_listView.setPresenter(nullptr);
    m_perioView.setPresenter(nullptr);
    m_financialView.setPresenter(nullptr);
    m_prescriptionView.setPresenter(nullptr);
}

void TabView::showFinancialView()
{
    showTabWidget(&m_financialView);
    m_listView.setPresenter(nullptr);
    m_perioView.setPresenter(nullptr);
    m_summaryView.setPresenter(nullptr);
    m_prescriptionView.setPresenter(nullptr);
}

void TabView::showPerscriptionView()
{
    showTabWidget(&m_prescriptionView);
    m_listView.setPresenter(nullptr);
    m_perioView.setPresenter(nullptr);
    m_summaryView.setPresenter(nullptr);
    m_financialView.setPresenter(nullptr);
}

void TabView::showWelcomeScreen()
{
    showTabWidget(welcomeScreen);
    m_listView.setPresenter(nullptr);
    m_perioView.setPresenter(nullptr);
    m_summaryView.setPresenter(nullptr);
    m_financialView.setPresenter(nullptr);
    m_prescriptionView.setPresenter(nullptr);
}

TabView::~TabView()
{
    ui.scrollArea->takeWidget();

    delete welcomeScreen;
}

void TabView::setWelcomeScreen(QWidget* welcomeScreen)
{
    this->welcomeScreen = welcomeScreen;
    showWelcomeScreen();
}



