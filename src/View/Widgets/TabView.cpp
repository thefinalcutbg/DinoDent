﻿#include "TabView.h"
#include <QAbstractScrollArea>
#include <QScrollBar>
#include "View/Theme.h"
#include "View/SubWidgets/TabTitle.h"
#include "Presenter/TabPresenter.h"
#include "View/CommonIcon.h"
#include "View/Widgets/DinoDent.h"
#include "View/Widgets/GlobalWidgets.h"

TabView::TabView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    ui.tabBar->setExpanding(false);
    ui.tabBar->setMovable(true);
    ui.tabBar->setTabsClosable(false);
    ui.tabBar->setElideMode(Qt::TextElideMode::ElideNone);

#ifdef Q_OS_MAC
    ui.tabBar->setStyle(Theme::fusionStyle());
#endif

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




    connect(ui.tabBar, &QTabBar::currentChanged, this,
        [=, this](int index)
        {
            if (index == -1)
            {
                TabPresenter::get().setCurrentTab(index);
                return;
            }

            auto tabId = static_cast<TabTitle*>
                (ui.tabBar->tabButton(index, QTabBar::ButtonPosition::RightSide))
                    ->getTabId();

            TabPresenter::get().setCurrentTab(tabId);
        });


    ui.scrollArea->setAlignment(Qt::AlignHCenter);
 
    ui.scrollArea->setObjectName("ScrollArea");
    setStyleSheet("#ScrollArea{background-color:"+ Theme::colorToString(Theme::background) + "}");

    TabPresenter::get().setView(this);
}


void TabView::requestClose(int tabId)
{
    TabPresenter::get().closeTabRequested(tabId);
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
    
    emit ui.tabBar->currentChanged(-1);

}

void TabView::newTab(int tabId, const TabName& tabName)
{
    TabTitle* tab = new TabTitle(this, tabId);

    tab->setMinimumWidth(200);

    tab->setHeaderIcon(CommonIcon::getPixmap(tabName.header_icon));
    tab->setFooterIcon(CommonIcon::getPixmap(tabName.footer_icon));

    tab->setText(QString::fromStdString(tabName.header), QString::fromStdString(tabName.footer));

    {
        QSignalBlocker blocker(ui.tabBar);

        int newTabIdx = ui.tabBar->addTab("");

        ui.tabBar->setTabButton(newTabIdx, QTabBar::ButtonPosition::RightSide, tab);

        

    }

    //if the tab, which is added is the first one:
    if (ui.tabBar->count() == 1) emit ui.tabBar->currentChanged(0);
    


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

void TabView::changeTabName(const TabName& tabName, int tabId)
{
    int tabIndex = getTabIndex(tabId);

    if (tabIndex == -1) return;

    auto tab = static_cast<TabTitle*>
        (ui.tabBar->tabButton(tabIndex, QTabBar::ButtonPosition::RightSide));

    tab->setHeaderIcon(CommonIcon::getPixmap(tabName.header_icon));
    tab->setFooterIcon(CommonIcon::getPixmap(tabName.footer_icon));

    tab->setText(QString::fromStdString(tabName.header), QString::fromStdString(tabName.footer));
    
    //re-setting the tab into the tabbar, to change the layout
    ui.tabBar->setTabButton(tabIndex, QTabBar::ButtonPosition::RightSide, nullptr);
    ui.tabBar->setTabButton(tabIndex, QTabBar::ButtonPosition::RightSide, tab);
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
    GlobalWidgets::mainWindow->disableButtons(false, false, false);
    showTabWidget(&m_listView);
    m_perioView.setPresenter(nullptr);
    m_summaryView.setPresenter(nullptr);
    m_prescriptionView.setPresenter(nullptr);
    m_financialView.setPresenter(nullptr);
    m_calendarView.setCalendarPresenter(nullptr);
}

void TabView::showPerioView()
{
    GlobalWidgets::mainWindow->disableButtons(true, false, true);
    showTabWidget(&m_perioView);
    m_listView.setPresenter(nullptr);
    m_summaryView.setPresenter(nullptr);
    m_prescriptionView.setPresenter(nullptr);
    m_financialView.setPresenter(nullptr);
    m_calendarView.setCalendarPresenter(nullptr);

}

void TabView::showSummaryView()
{
    GlobalWidgets::mainWindow->disableButtons(true, true, true);
    showTabWidget(&m_summaryView);
    m_listView.setPresenter(nullptr);
    m_perioView.setPresenter(nullptr);
    m_financialView.setPresenter(nullptr);
    m_prescriptionView.setPresenter(nullptr);
    m_calendarView.setCalendarPresenter(nullptr);
}

void TabView::showFinancialView()
{
    GlobalWidgets::mainWindow->disableButtons(false, false, false);
    showTabWidget(&m_financialView);
    m_listView.setPresenter(nullptr);
    m_perioView.setPresenter(nullptr);
    m_summaryView.setPresenter(nullptr);
    m_prescriptionView.setPresenter(nullptr);
    m_calendarView.setCalendarPresenter(nullptr);
}

void TabView::showPerscriptionView()
{
    
    showTabWidget(&m_prescriptionView);
    m_listView.setPresenter(nullptr);
    m_perioView.setPresenter(nullptr);
    m_summaryView.setPresenter(nullptr);
    m_financialView.setPresenter(nullptr);
    m_calendarView.setCalendarPresenter(nullptr);
    GlobalWidgets::mainWindow->disableButtons(false, false, false);
}

void TabView::showCalendarView()
{
    GlobalWidgets::mainWindow->disableButtons(true, true, true);
    showTabWidget(&m_calendarView);
    m_listView.setPresenter(nullptr);
    m_perioView.setPresenter(nullptr);
    m_summaryView.setPresenter(nullptr);
    m_financialView.setPresenter(nullptr);
    m_prescriptionView.setPresenter(nullptr);    
}

void TabView::showWelcomeScreen()
{
    GlobalWidgets::mainWindow->disableButtons(true, true, true);
    showTabWidget(&welcomeScreen);
    m_listView.setPresenter(nullptr);
    m_perioView.setPresenter(nullptr);
    m_summaryView.setPresenter(nullptr);
    m_financialView.setPresenter(nullptr);
    m_prescriptionView.setPresenter(nullptr);
}

TabView::~TabView()
{
    ui.scrollArea->takeWidget();

}



