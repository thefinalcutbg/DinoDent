#include "PerioView.h"
#include "Model/PerioStatus.h"
#include "PerioView/PerioScene.h"
#include <QButtonGroup>
#include <QPainter>
#include <QDebug>

PerioView::PerioView(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    
    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(ui.upperButton);
    group->addButton(ui.lowerButton);
    group->setExclusive(true);

    ui.upperButton->setChecked(true);
    connect(ui.upperButton, &QPushButton::clicked, [&] { ui.stackedWidget->setCurrentWidget(ui.perioUpper); });
    connect(ui.lowerButton, &QPushButton::clicked, [&] { ui.stackedWidget->setCurrentWidget(ui.perioLower); });
}

void PerioView::setPresenter(PerioPresenter* presenter)
{
    this->presenter = presenter;
}


#include "View/ToothPaintDevices/PaintHint.h"

void PerioView::setToothHint(const ToothPaintHint& hint)
{
    if (hint.idx < 16) ui.perioUpper->setTooth(hint);
    else ui.perioLower->setTooth(hint);
}

void PerioView::setPerioStatus(const PerioStatus& upper, const PerioStatus& lower)
{
    ui.perioUpper->setPerioStatus(upper);
    ui.perioLower->setPerioStatus(lower);
}

std::tuple<PerioStatus, PerioStatus> PerioView::getPerioStatus()
{
    return std::make_tuple(ui.perioUpper->getPerioStatus(), ui.perioLower->getPerioStatus());
}

void PerioView::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(0, 0, width(), height(), Qt::GlobalColor::white);
}