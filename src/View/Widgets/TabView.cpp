#include "TabView.h"
#include <QAbstractScrollArea>
#include <QScrollBar>
#include <QWindow>
#include "View/Theme.h"
#include "View/SubWidgets/TabTitle.h"
#include "Presenter/TabPresenter.h"
#include "View/CommonIcon.h"
#include "View/Widgets/DinoDent.h"
#include "View/Widgets/GlobalWidgets.h"
#include "Model/User.h"

#include <QScrollArea>
#include <QPropertyAnimation>
#include <QWheelEvent>
#include <QApplication>
#include <QPointer>
#include <algorithm>

class SmoothWheelScroll : public QObject
{
public:
    explicit SmoothWheelScroll(QScrollArea* area, QObject* parent = nullptr)
        : QObject(parent), m_area(area)
    {
        auto* sb = m_area->verticalScrollBar();
        m_anim = new QPropertyAnimation(sb, "value", this);
        m_anim->setEasingCurve(QEasingCurve::OutCubic);
        m_anim->setDuration(180);
        m_target = sb->value();

        connect(sb, &QScrollBar::rangeChanged, this, [this](int, int) {
            if (!m_area) return;
            auto* sb2 = m_area->verticalScrollBar();
            m_anim->stop();
            m_target = sb2->value();
            });

        qApp->installEventFilter(this);
    }

    ~SmoothWheelScroll() override
    {
        if (qApp) qApp->removeEventFilter(this);
    }

    void setPixelsPerStep(int px) { m_pxPerStep = std::max(1, px); }
    void setDurationMs(int ms) { m_anim->setDuration(std::max(1, ms)); }

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override
    {
        if (!m_area) return QObject::eventFilter(obj, ev);
        if (ev->type() != QEvent::Wheel) return QObject::eventFilter(obj, ev);

        auto* w = qobject_cast<QWidget*>(obj);
        if (!w) return QObject::eventFilter(obj, ev);

        auto* vp = m_area->viewport();
        if (!(w == vp || vp->isAncestorOf(w))) return QObject::eventFilter(obj, ev);

        auto* we = static_cast<QWheelEvent*>(ev);

        const QPoint pd = we->pixelDelta();
        const QPoint ad = we->angleDelta();

        if (!pd.isNull()) return QObject::eventFilter(obj, ev);
        if (ad.y() == 0) return QObject::eventFilter(obj, ev);

        auto* sb = m_area->verticalScrollBar();

        if (m_anim->state() != QAbstractAnimation::Running)
            m_target = sb->value();

        const double steps = ad.y() / 120.0;
        m_target -= int(steps * m_pxPerStep);

        m_target = std::clamp(m_target, sb->minimum(), sb->maximum());

        m_anim->stop();
        m_anim->setStartValue(sb->value());
        m_anim->setEndValue(m_target);
        m_anim->start();

        we->accept();
        return true;
    }

private:
    QPointer<QScrollArea> m_area;
    QPropertyAnimation* m_anim = nullptr;
    int m_target = 0;
    int m_pxPerStep = 60;
};


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

    auto* smooth = new SmoothWheelScroll(ui.scrollArea, ui.scrollArea);
    smooth->setPixelsPerStep(3 * fontMetrics().height());
    smooth->setDurationMs(120);

    ui.tabBar->setExpanding(false);
    ui.tabBar->setMovable(true);
    ui.tabBar->setTabsClosable(false);
    ui.tabBar->setElideMode(Qt::TextElideMode::ElideNone);

#ifdef Q_OS_MAC
    ui.tabBar->setStyle(Theme::fusionStyle());
    ui.tabBar->installEventFilter(new NoHScrollFilter(ui.tabBar));
#endif

    setCustomStyleSheet(false);

    static auto showFocusedTabBorder = false;

    connect(ui.scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int value)
    {
            if (value > 10 == showFocusedTabBorder) return;

            showFocusedTabBorder = value > 10;

            setCustomStyleSheet(showFocusedTabBorder);
       
    });

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
    ui.scrollArea->setAlignment(Qt::AlignHCenter);

    ui.scrollArea->setObjectName("ScrollArea");
    setStyleSheet("#ScrollArea{background-color:" + Theme::colorToString(Theme::background) + "}");

    TabPresenter::get().setView(this);
}

void TabView::disableViewportUpdates()
{
    scrollArea()->viewport()->setUpdatesEnabled(false);
}

void TabView::enableViewportUpdates()
{
    if (auto w = scrollArea()->widget())
        if (w->layout()) w->layout()->activate();

    scrollArea()->viewport()->setUpdatesEnabled(true);
    scrollArea()->viewport()->update();
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

void TabView::setCustomStyleSheet(bool focusedTabBorder)
{
    const QString overlap      = focusedTabBorder ? "0px"  : "-2px";
    const QString padBottom    = focusedTabBorder ? "1px"  : "3px";
    const QString bottomBorder = focusedTabBorder
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


void TabView::showTabWidget(QWidget* w)
{
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
    return
    {
        ui.scrollArea->verticalScrollBar()->value(),
        ui.scrollArea->horizontalScrollBar()->value()
    };
}

void TabView::setScrollPos(std::pair<int, int> scrollPos)
{
    ui.scrollArea->verticalScrollBar()->setValue(scrollPos.first);
    ui.scrollArea->horizontalScrollBar()->setValue(scrollPos.second);
}

void TabView::showListView()
{
    GlobalWidgets::mainWindow->disableButtons(false, false, false);
    showTabWidget(&m_listView);
    m_listView.scheduleBake();
    m_perioView.setPresenter(nullptr);
    m_prescriptionView.setPresenter(nullptr);
    m_financialView.setPresenter(nullptr);
    m_calendarView.setCalendarPresenter(nullptr);
    m_treatmentView.setPresenter(nullptr);
}

void TabView::showPerioView()
{
    GlobalWidgets::mainWindow->disableButtons(true, false, true);
    showTabWidget(&m_perioView);
	m_perioView.scheduleBake();
    m_listView.setPresenter(nullptr);
    m_prescriptionView.setPresenter(nullptr);
    m_financialView.setPresenter(nullptr);
    m_calendarView.setCalendarPresenter(nullptr);
    m_treatmentView.setPresenter(nullptr);
}


void TabView::showFinancialView()
{
    GlobalWidgets::mainWindow->disableButtons(false, false, false);
    showTabWidget(&m_financialView);
    m_financialView.scheduleBake();
    m_listView.setPresenter(nullptr);
    m_perioView.setPresenter(nullptr);
    m_prescriptionView.setPresenter(nullptr);
    m_calendarView.setCalendarPresenter(nullptr);
    m_treatmentView.setPresenter(nullptr);
}

void TabView::showPerscriptionView()
{
    GlobalWidgets::mainWindow->disableButtons(false, false, false);
    showTabWidget(&m_prescriptionView);
	m_prescriptionView.scheduleBake();
    m_listView.setPresenter(nullptr);
    m_perioView.setPresenter(nullptr);
    m_financialView.setPresenter(nullptr);
    m_calendarView.setCalendarPresenter(nullptr);
    m_treatmentView.setPresenter(nullptr);
}

void TabView::showCalendarView()
{
    GlobalWidgets::mainWindow->disableButtons(true, true, true);
    showTabWidget(&m_calendarView);
    m_listView.setPresenter(nullptr);
    m_perioView.setPresenter(nullptr);
    m_financialView.setPresenter(nullptr);
    m_prescriptionView.setPresenter(nullptr);
    m_treatmentView.setPresenter(nullptr);
}

void TabView::showWelcomeScreen()
{
    GlobalWidgets::mainWindow->disableButtons(true, true, true);
    welcomeScreen.refreshTip();
    showTabWidget(&welcomeScreen);
    m_listView.setPresenter(nullptr);
    m_perioView.setPresenter(nullptr);
    m_financialView.setPresenter(nullptr);
    m_prescriptionView.setPresenter(nullptr);
    m_treatmentView.setPresenter(nullptr);
    m_calendarView.setCalendarPresenter(nullptr);
}

void TabView::showTreatmentPlanView()
{
    GlobalWidgets::mainWindow->disableButtons(false, false, false);
    showTabWidget(&m_treatmentView);
	m_treatmentView.scheduleBake();
    m_listView.setPresenter(nullptr);
    m_perioView.setPresenter(nullptr);
    m_financialView.setPresenter(nullptr);
    m_prescriptionView.setPresenter(nullptr);
    m_calendarView.setCalendarPresenter(nullptr);
}

TabView::~TabView()
{
    ui.scrollArea->takeWidget();

}
