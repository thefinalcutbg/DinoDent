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
                tabPresenter->setCurrentList(index);
                return;
            }

            int vecPos = ui.tabBar->tabData(index).toInt();

            tabPresenter->setCurrentList(vecPos);
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
    setStyleSheet("#ScrollArea{background-color : white;}");
    ui.scrollArea->setWidget(noTabs);
    ui.scrollArea->setFrameStyle(QFrame::NoFrame);

    
    
}

TabView::~TabView()
{
}

void TabView::newTab(int vecPos, std::string tabName)
{
    if (!ui.tabBar->count())
    {
        ui.scrollArea->takeWidget();
        ui.scrollArea->setWidget(&_listView);
    }


    ui.tabBar->addNewTab(QString::fromStdString(tabName), vecPos);
    ui.tabBar->setCurrentIndex(ui.tabBar->count() - 1);

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

    if (!ui.tabBar->count())
    {
        ui.scrollArea->takeWidget();
        ui.scrollArea->setWidget(noTabs);
    }
        
}

void TabView::removeTab(int vecPos)
{
    for (int i = 0; i < ui.tabBar->count(); i++)
    {
        if (ui.tabBar->tabData(i) == vecPos)
        {
            ui.tabBar->closeTab(i);

            if (!ui.tabBar->count())
            {
                ui.scrollArea->takeWidget();
                ui.scrollArea->setWidget(noTabs);
            }
        }

    }
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

IListView* TabView::listView()
{
    return &_listView;
}



