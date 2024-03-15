#include "CalendarWidget.h"
#include <QTextCharFormat>
#include <QFile>
#include <QMenu>
#include <QFile>
#include <QTextCharFormat>
#include <QCalendarWidget>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QWheelEvent>
#include <QDateTimeEdit>

CalendarWidget::CalendarWidget(QWidget* parent) : QCalendarWidget(parent) {

    setLocale(QLocale(QLocale::Bulgarian));
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames);
    setFirstDayOfWeek(Qt::Monday);
    setNavigationBarVisible(false);
    QTextCharFormat dateCharFormat;
    dateCharFormat.setBackground(Qt::white);
    QTextCharFormat headerCharFormat;
    headerCharFormat.setBackground(Qt::white);
    headerCharFormat.setForeground(Qt::gray);
    setHeaderTextFormat(headerCharFormat);

    for (int dayOfWeek = Qt::Monday; dayOfWeek <= Qt::Sunday; dayOfWeek++) {
        setWeekdayTextFormat(Qt::DayOfWeek(dayOfWeek), dateCharFormat);
    }

    QFile dateEditFile(":/style/dateEditStyle.qss");

    if (dateEditFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(dateEditFile.readAll());
        setStyleSheet(styleSheet);
        dateEditFile.close();
    }

    QWidget * topWidget = new QWidget(this);
    topWidget->setObjectName("headerWidget");
    topWidget->setFixedHeight(40);
    topWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout * hboxLayout = new QHBoxLayout();
    hboxLayout->setContentsMargins(12, 0, 12, 0);
    hboxLayout->setSpacing(4);

    prevYearButton = new QPushButton(topWidget);
    prevYearButton->setObjectName("prevYearButton");
    prevYearButton->setFixedSize(16, 16);
    prevYearButton->setToolTip("Предишна година");

    nextYearButton = new QPushButton(topWidget);
    nextYearButton->setObjectName("nextYearButton");
    nextYearButton->setFixedSize(16, 16);
    nextYearButton->setToolTip("Следваща година");

    prevMonthButton = new QPushButton(topWidget);
    prevMonthButton->setObjectName("prevMonthButton");
    prevMonthButton->setFixedSize(16, 16);
    prevMonthButton->setToolTip("Предишен месец");

    nextMonthButton = new QPushButton(topWidget);
    nextMonthButton->setObjectName("nextMonthButton");
    nextMonthButton->setFixedSize(16, 16);
    nextMonthButton->setToolTip("Следващ месец");

    yearButton = new QPushButton(topWidget);
    yearButton->setObjectName("yearButton");

    monthButton = new QPushButton(topWidget);
    monthButton->setObjectName("monthButton");

    setDateLabelText(QDate::currentDate().year(), QDate::currentDate().month());

    hboxLayout->addWidget(prevYearButton);
    hboxLayout->addWidget(prevMonthButton);
    hboxLayout->addStretch();
    hboxLayout->addWidget(yearButton);
    hboxLayout->addWidget(monthButton);
    hboxLayout->addStretch();
    hboxLayout->addWidget(nextMonthButton);
    hboxLayout->addWidget(nextYearButton);
    topWidget->setLayout(hboxLayout);

    vBodyLayout = qobject_cast<QVBoxLayout *>(this->layout());
    vBodyLayout->insertWidget(0, topWidget);

    monthMenu = new QWidget(this);
    monthMenu->setObjectName("monthMenu");
    vBodyLayout->insertWidget(1, monthMenu);
    monthMenu->hide();
    monthGridLayout = new QGridLayout();
    monthMenu->setLayout(monthGridLayout);
    monthGridLayout->setSpacing(0);
    monthMenu->installEventFilter(this);

    yearMenu = new QWidget(this);
    yearMenu->setObjectName("yearMenu");
    vBodyLayout->insertWidget(2, yearMenu);
    yearMenu->hide();
    yearGridLayout = new QGridLayout();
    yearMenu->setLayout(yearGridLayout);
    yearGridLayout->setSpacing(0);
    yearMenu->installEventFilter(this);

    changeType(type);

    connect(prevYearButton, &QPushButton::clicked, this, [&](){
        selectedYear(-1);
    });

    connect(nextYearButton, &QPushButton::clicked, this, [&](){
        selectedYear(1);
    });

    connect(prevMonthButton, &QPushButton::clicked, this, [&](){
        int year = selectedDate().year();
        int month = selectedDate().month();
        selectedMonth(year, month - 1);
    });
    connect(nextMonthButton, &QPushButton::clicked, this, [&](){
        int year = selectedDate().year();
        int month = selectedDate().month();
        selectedMonth(year, month + 1);
    });
    connect(this, &QCalendarWidget::currentPageChanged, this, [&](int year, int month) {
        setDateLabelText(year, month);
        selectedMonth(year, month);
        menuChanged();
    });

    connect(monthButton, &QPushButton::clicked, this, [&]() {
        if (monthMenu->isHidden()) {
            monthMenuPopup();
        } else {
            changeMenu(type);
        }
    });

    connect(yearButton, &QPushButton::clicked, this, [&]() {
        if (yearMenu->isHidden()) {
            yearMenuPopup();
        } else {
            changeMenu(type);
        }
    });


}

void CalendarWidget::setDateLabelText(int year, int month) {
    yearButton->setText(QStringLiteral("%1").arg(year));

    QLocale l(QLocale::Bulgarian);
    monthButton->setText(l.monthName(month, QLocale::ShortFormat));
}

void CalendarWidget::selectedMonth(int year, int month) {
    if (year >= minimumDate().year() && year <= maximumDate().year()) {
        QDate seletedDate = selectedDate(); // 当前选中的月份
        QDate validDate = seletedDate.addMonths(month - seletedDate.month());
        static_cast<QDateTimeEdit*>(parent()->parent())->setDate(validDate);
    }
}

void CalendarWidget::selectedYear(int yearCount) {
    QDate validDate = selectedDate().addYears(yearCount);
    static_cast<QDateTimeEdit*>(parent()->parent())->setDate(validDate);
}

void CalendarWidget::monthMenuPopup() {

    changeMenu(menuContent::MONTH);
    QList<QPushButton *> monthList;
    int currentMonth = selectedDate().month();
    int initializationMonth = currentMonth - 12;
    int minMonth = 0 - initializationMonth;
    int maxMonth = 13 + minMonth;
    int monthTableCount = 0;
    int monthCount = initializationMonth;

    for (int monthTableRow = 0; monthTableRow < 4; monthTableRow++) {
        for (int monthTableColumn = 0; monthTableColumn < 4; monthTableColumn++) {
            monthList << new QPushButton(this);
            int actualMonth = (monthCount <= 0)
                              ? (monthCount + 12) : ((monthCount) > 12)
                                    ? (monthCount % 12) : (monthCount);

            QLocale l(QLocale::Bulgarian);

            monthList[monthTableCount]->setText(l.monthName(actualMonth, QLocale::ShortFormat));
            if (monthTableCount == 12) {
                monthList[monthTableCount]->setObjectName("monthListSelected");
            } else if (monthTableCount > minMonth && monthTableCount < maxMonth) {
                monthList[monthTableCount]->setObjectName("monthListEnabled");
            } else if (monthTableCount <= minMonth || monthTableCount >= maxMonth) {
                monthList[monthTableCount]->setObjectName("monthListDisabled");
            }
            monthGridLayout->addWidget(monthList[monthTableCount], monthTableRow, monthTableColumn);
            connect(monthList[monthTableCount], &QPushButton::clicked, this, [=, this]() {
                int year = selectedDate().year();
                selectedMonth(year, monthCount);
                if (type == menuContent::MONTH) {
                    emit activated(selectedDate());
                } else {
                    changeMenu(menuContent::DAY);
                }
            });
            monthCount++;
            monthTableCount++;
        }
    }
}

void CalendarWidget::yearMenuPopup() {

    changeMenu(menuContent::YEAR);

    QList<QPushButton *> yearList;

    int currentYear = selectedDate().year();
    int initializationYear = currentYear - 12;
    int yearTableCount = 0;
    int yearCount = initializationYear;

    for (int yearTableRow = 0; yearTableRow < 4; yearTableRow++) {

        for (int yearTableColumn = 0; yearTableColumn < 4; yearTableColumn++) {

            yearList << new QPushButton(this);
            yearList[yearTableCount]->setText(QStringLiteral("%1").arg(yearCount));

            if (yearTableCount == 12) {
                yearList[yearTableCount]->setObjectName("yearListSelected");
            } else if (yearTableCount <= 2 || yearTableCount >= 13) {
                yearList[yearTableCount]->setObjectName("yearListDisabled");
            } else if (yearTableCount > 2 && yearTableCount < 13) {
                yearList[yearTableCount]->setObjectName("yearListEnabled");
            }

            yearGridLayout->addWidget(yearList[yearTableCount], yearTableRow, yearTableColumn);

            connect(yearList[yearTableCount], &QPushButton::clicked, this, [=, this]() {
                int currentYear = selectedDate().year();
                selectedYear(yearCount - currentYear);
                if (type == menuContent::YEAR) {
                    emit activated(selectedDate());
                } else {
                    changeMenu(type);
                }
            });

            yearCount++;
            yearTableCount++;
        }
    }
}

void CalendarWidget::menuChanged() {
    if (!yearMenu->isHidden()) {
        yearMenuPopup();
    } else if (!monthMenu->isHidden()) {
        monthMenuPopup();
    }
}

void CalendarWidget::changeMenu(int sequence) {
    vBodyLayout->itemAt(4)->widget()->hide();
    yearMenu->hide();
    monthMenu->hide();
    switch (sequence) {
    case menuContent::YEAR: yearMenu->show();
        yearButton->setStyleSheet("border: 1px solid gray;");
        monthButton->setStyleSheet("border: 0;");
        break;
    case menuContent::MONTH: monthMenu->show();
        monthButton->setStyleSheet("border: 1px solid gray;");
        yearButton->setStyleSheet("border: 0;");
        break;
    case menuContent::DAY: vBodyLayout->itemAt(4)->widget()->show();
        monthButton->setStyleSheet("border: 0;");
        yearButton->setStyleSheet("border: 0;");
        break;
    }
}

bool CalendarWidget::eventFilter(QObject *widget, QEvent *event) {

    if (widget == yearMenu || widget == monthMenu) {
        if (event->type() == QEvent::Wheel) {
            QWheelEvent * wheelEvent = static_cast<QWheelEvent *>(event);
            int angelDeltaY = wheelEvent->angleDelta().y();
            int year = selectedDate().year();
            int month = selectedDate().month();
            if (widget == yearMenu && angelDeltaY > 0) {
                selectedYear(-4);
            } else if (widget == yearMenu && angelDeltaY < 0) {
                selectedYear(4);
            } else if (widget == monthMenu && angelDeltaY > 0) {
                selectedMonth(year, month - 4);
            } else if (widget == monthMenu && angelDeltaY < 0) {
                selectedMonth(year, month + 4);
            }
            menuChanged();
        }
    }
    return true;
}

void CalendarWidget::changeType(menuContent type) {
    this->type = type;
    if (type == menuContent::DAY) {
        changeMenu(menuContent::DAY);
       // setDisplayFormat("dd.MM.yyyy");
        monthButton->show();
    } else if (type == menuContent::MONTH) {
        monthMenuPopup();
      //  setDisplayFormat("MM.yyyy");
        monthButton->show();
    } else if (type == menuContent::YEAR) {
        yearMenuPopup();
      //  setDisplayFormat("yyyy");
        monthButton->hide();
    }
}
