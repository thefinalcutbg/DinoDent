#include "TabView.h"

#include <QScrollBar>

#include "Model/User.h"
#include "Presenter/TabPresenter.h"
#include "View/Theme.h"
#include "View/CommonIcon.h"
#include "View/SubWidgets/TabTitle.h"
#include "View/Widgets/DinoDent.h"
#include "View/Widgets/GlobalWidgets.h"
#include "View/uiComponents/SmoothWheelScroll.h"

class NoHScrollFilter : public QObject {
public:
    using QObject::QObject;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::Wheel) {
            auto *we = static_cast<QWheelEvent*>(event);

            const QPoint ad = we->angleDelta();
            const QPoint pd = we->pixelDelta();

            const bool hasHoriz =
                (ad.x() != 0) || (pd.x() != 0);

            if (hasHoriz) {
                we->ignore();
                return true;
            }
        }
        return QObject::eventFilter(obj, event);
    }
};

TabView::TabView(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    ui.tabBar->setExpanding(false);
    ui.tabBar->setMovable(true);
    ui.tabBar->setTabsClosable(false);
    ui.tabBar->setElideMode(Qt::TextElideMode::ElideNone);

#ifdef Q_OS_MAC
    ui.tabBar->setStyle(Theme::fusionStyle());
    ui.tabBar->installEventFilter(new NoHScrollFilter(ui.tabBar));
#endif

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

    ui.tabBar->setDrawBase(false);

    initTabs();

    refreshTabBorder(nullptr);

    TabPresenter::get().setView(this);
}

void TabView::disableViewportUpdates(TabType t)
{
	static_cast<QScrollArea*>(ui.stackedWidget->widget(static_cast<int>(t)))->viewport()->setUpdatesEnabled(false);
}

void TabView::enableViewportUpdates(TabType t)
{
    auto scrollArea = static_cast<QScrollArea*>(ui.stackedWidget->widget(static_cast<int>(t)));

    if (auto w = scrollArea->widget())
        if (w->layout()) w->layout()->activate();

    scrollArea->viewport()->setUpdatesEnabled(true);
    scrollArea->viewport()->update();
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

void TabView::refreshTabBorder(QScrollArea* sa)
{

    bool value = false;
    
    if (sa) {
        value = sa->verticalScrollBar()->value() > 10;
        if (value == showFocusedTabBorder) return;
    }
	
    showFocusedTabBorder = value;

    const QString overlap = showFocusedTabBorder ? "0px" : "-2px";
    const QString padBottom = showFocusedTabBorder ? "1px" : "3px";
    const QString bottomBorder = showFocusedTabBorder
        ? ("1px solid " + Theme::colorToString(Theme::border))
        : ("1px solid " + Theme::colorToString(Theme::background));

    ui.tabBar->setStyleSheet(
        "QTabBar {"
        "  border: none;"
        "  background: " + Theme::colorToString(Theme::mainBackgroundColor) + ";"
        "  border-bottom: 1px solid " + Theme::colorToString(Theme::border) + ";"
        "}"

        "QTabBar::tab {"
        "  background-color: " + Theme::colorToString(Theme::inactiveTabBG) + ";"
        "  border: 1px solid transparent;"
        "  border-top-left-radius: 8px;"
        "  border-top-right-radius: 8px;"
        "  padding: 1px 0px;"
        "  margin-bottom: 0px;"
        "}"

        "QTabBar::tab:selected {"
        "  background-color: " + Theme::colorToString(Theme::background) + ";"
        "  border-top-color: " + Theme::colorToString(Theme::border) + ";"
        "  border-left-color: " + Theme::colorToString(Theme::border) + ";"
        "  border-right-color: " + Theme::colorToString(Theme::border) + ";"
        "  margin-bottom: " + overlap + ";"
        "  padding-bottom: " + padBottom + ";"
        "  border-bottom: " + bottomBorder + ";"
        "}"
    );
}

void TabView::initTabs()
{
    auto addPage = [this](TabType type, QWidget* view)
    {
            auto sa = new QScrollArea(ui.stackedWidget);
            sa->setWidgetResizable(true);
            sa->setFrameShape(QFrame::NoFrame);

            sa->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

            sa->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            sa->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            sa->setWidget(view);

            sa->setObjectName("ScrollArea");
            sa->setStyleSheet("#ScrollArea{background-color:" + Theme::colorToString(Theme::background) + "}");

            auto smooth = new SmoothWheelScroll(sa, sa);
            smooth->setPixelsPerWheelStep(3.5 * fontMetrics().height());

            connect(sa->verticalScrollBar(), &QScrollBar::valueChanged,
                this, [=](int value)
                {
					if (!sa->isVisible()) return;
    
					refreshTabBorder(sa);
				});

            ui.stackedWidget->addWidget(sa);
    };

	m_listView = new ListView(nullptr);
	m_perioView = new PerioView(nullptr);
	m_financialView = new FinancialView(nullptr);
	m_prescriptionView = new PrescriptionView(nullptr);
	m_treatmentView = new TreatmentPlanView(nullptr);
	m_calendarView = new CalendarView(nullptr);
	welcomeScreen = new WelcomeWidget(nullptr);

	addPage(TabType::PatientSummary, welcomeScreen);
	addPage(TabType::AmbList, m_listView);
    addPage(TabType::Prescription, m_prescriptionView);
	addPage(TabType::PerioStatus, m_perioView);
    addPage(TabType::TreatmentPlan, m_treatmentView);
	addPage(TabType::Financial, m_financialView);
	addPage(TabType::Calendar, m_calendarView);
  
}

void TabView::showView(TabType t)
{
    ShadowBakeWidget* shadowWidget = nullptr;

    auto scrollArea = qobject_cast<QScrollArea*>(ui.stackedWidget->widget(static_cast<int>(t)));

    if (t != TabType::PatientSummary && t != TabType::Calendar) {

        shadowWidget = qobject_cast<ShadowBakeWidget*>(scrollArea->widget());

    }

    if (shadowWidget) shadowWidget->scheduleBake();
    
    refreshTabBorder(scrollArea);

    ui.stackedWidget->setCurrentIndex(static_cast<int>(t));

    if (t == TabType::PerioStatus) m_perioView->disableBaking(true);

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

    //    tab->setMinimumWidth(200);

    tab->setData(tabName);

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

    tab->setData(tabName);

    //re-setting the tab into the tabbar, to change the layout
    ui.tabBar->setTabButton(tabIndex, QTabBar::ButtonPosition::RightSide, nullptr);
    ui.tabBar->setTabButton(tabIndex, QTabBar::ButtonPosition::RightSide, tab);
}

std::pair<int, int> TabView::getScrollPos()
{
    auto sa = scrollArea();
    if (!sa) return { 0, 0 };

    return {
        sa->verticalScrollBar()->value(),
        sa->horizontalScrollBar()->value()
    };
}

void TabView::setScrollPos(std::pair<int, int> scrollPos)
{
    auto sa = scrollArea();
    if (!sa) return;

    sa->verticalScrollBar()->setValue(scrollPos.first);
    sa->horizontalScrollBar()->setValue(scrollPos.second);
}

QScrollArea* TabView::scrollArea()
{
    return qobject_cast<QScrollArea*>(ui.stackedWidget->currentWidget());
}

void TabView::showListView()
{
    GlobalWidgets::mainWindow->disableButtons(false, false, false);
	showView(TabType::AmbList);

    m_perioView->setPresenter(nullptr);
    m_prescriptionView->setPresenter(nullptr);
    m_financialView->setPresenter(nullptr);
    m_calendarView->setCalendarPresenter(nullptr);
    m_treatmentView->setPresenter(nullptr);
}

void TabView::showPerioView()
{
    GlobalWidgets::mainWindow->disableButtons(true, false, true);
    showView(TabType::PerioStatus);

    m_listView->setPresenter(nullptr);
    m_prescriptionView->setPresenter(nullptr);
    m_financialView->setPresenter(nullptr);
    m_calendarView->setCalendarPresenter(nullptr);
    m_treatmentView->setPresenter(nullptr);
}

void TabView::showFinancialView()
{
    GlobalWidgets::mainWindow->disableButtons(false, false, false);
    showView(TabType::Financial);

    m_listView->setPresenter(nullptr);
    m_perioView->setPresenter(nullptr);
    m_prescriptionView->setPresenter(nullptr);
    m_calendarView->setCalendarPresenter(nullptr);
    m_treatmentView->setPresenter(nullptr);
}

void TabView::showPerscriptionView()
{
    GlobalWidgets::mainWindow->disableButtons(false, false, false);
	showView(TabType::Prescription);

    m_listView->setPresenter(nullptr);
    m_perioView->setPresenter(nullptr);
    m_financialView->setPresenter(nullptr);
    m_calendarView->setCalendarPresenter(nullptr);
    m_treatmentView->setPresenter(nullptr);
}

void TabView::showCalendarView()
{
    GlobalWidgets::mainWindow->disableButtons(true, true, true);
	showView(TabType::Calendar);
    m_listView->setPresenter(nullptr);
    m_perioView->setPresenter(nullptr);
    m_financialView->setPresenter(nullptr);
    m_prescriptionView->setPresenter(nullptr);
    m_treatmentView->setPresenter(nullptr);
}

void TabView::showWelcomeScreen()
{
    GlobalWidgets::mainWindow->disableButtons(true, true, true);
    welcomeScreen->refreshTip();
	showView(TabType::PatientSummary);

    m_listView->setPresenter(nullptr);
    m_perioView->setPresenter(nullptr);
    m_financialView->setPresenter(nullptr);
    m_prescriptionView->setPresenter(nullptr);
    m_treatmentView->setPresenter(nullptr);
    m_calendarView->setCalendarPresenter(nullptr);
}

void TabView::showTreatmentPlanView()
{
    GlobalWidgets::mainWindow->disableButtons(false, false, false);
	showView(TabType::TreatmentPlan);


    m_listView->setPresenter(nullptr);
    m_perioView->setPresenter(nullptr);
    m_financialView->setPresenter(nullptr);
    m_prescriptionView->setPresenter(nullptr);
    m_calendarView->setCalendarPresenter(nullptr);
}

TabView::~TabView()
{}
