#include "FurcationWidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include "Model/Dental/ToothUtils.h"
#include <QLabel>
#include <QPainter>

FurcationWidget::FurcationWidget(QWidget *parent)
	: QWidget(parent)
{
//	ui.setupUi(this);

    QVBoxLayout* verticalLayout;
    QHBoxLayout* horizontalLayout;

    setObjectName(QString::fromUtf8("FurcationWidget"));

    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(0);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(0);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    leftButton = new PerioStateButton(this);
    leftButton->setObjectName(QString::fromUtf8("leftButton"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    leftButton->setSizePolicy(sizePolicy);

    horizontalLayout->addWidget(leftButton);

    rightButton = new PerioStateButton(this);
    rightButton->setObjectName(QString::fromUtf8("rightButton"));
    rightButton->setSizePolicy(sizePolicy);

    horizontalLayout->addWidget(rightButton);


    verticalLayout->addLayout(horizontalLayout);

    downButton = new PerioStateButton(this);
    downButton->setObjectName(QString::fromUtf8("downButton"));
    downButton->setSizePolicy(sizePolicy);

    verticalLayout->addWidget(downButton);

    dummy = new QLabel(this);
    dummy->setObjectName(QString::fromUtf8("downButton"));
    dummy->setSizePolicy(sizePolicy);

    dummy->hide();
    leftButton->hide();
    rightButton->hide();
    downButton->hide();
    

    QLabel* dummy = new QLabel("", this);
    dummy->setFixedSize(0, 0);

    verticalLayout->addWidget(dummy);

    setMinimumHeight(60);

    setSizePolicy(sizePolicy);

    
    leftButton->setAvailableStates(&states);
    rightButton->setAvailableStates(&states);
    downButton->setAvailableStates(&states);

    connect(leftButton, &QAbstractButton::clicked, this, [=, this] { emit valueChanged(getMeasurment());});
    connect(rightButton, &QAbstractButton::clicked, this, [=, this] { emit valueChanged(getMeasurment());});
    connect(downButton, &QAbstractButton::clicked, this, [=, this] { emit valueChanged(getMeasurment());});
}



void FurcationWidget::setIndex(int toothIndex)
{
    auto type = ToothUtils::getToothType(toothIndex);

    if (type == Dental::Type::Molar && toothIndex < 16)
    {
        leftButton->show();
        rightButton->show();
        downButton->show();
        return;
    }

    if (type == Dental::Type::Premolar && toothIndex < 16)
    {
        leftButton->show();
        rightButton->show();
        return;
    }


    if (type == Dental::Type::Molar && toothIndex > 15)
    {
        leftButton->show();
        downButton->show();
        return;
    }

    dummy->show();
    
}


FurcationMeasurment FurcationWidget::getMeasurment()
{
    return FurcationMeasurment{
        leftButton->getState(),
        rightButton->getState(),
        downButton->getState() };
}

void FurcationWidget::setMeasurment(int a, int b, int c)
{
    leftButton->setState(a);
    rightButton->setState(b);
    downButton->setState(c);
}

void FurcationWidget::paintEvent(QPaintEvent*)
{

    QPainter painter(this);

    QColor color;
    if (!isEnabled()) color = QColor(Qt::GlobalColor::lightGray);
    else color = QColor(Qt::GlobalColor::white);


    QRectF rect(0, 0, width(), height());

    painter.fillRect(rect, color);

    QPen pen; pen.setColor(QColor(Qt::GlobalColor::gray));
    painter.setPen(pen);
    painter.drawRect(rect);

}

FurcationWidget::~FurcationWidget()
{
}
