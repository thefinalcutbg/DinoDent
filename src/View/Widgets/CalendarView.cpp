#include "CalendarView.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QDesktopServices>
#include <QOAuthHttpServerReplyHandler>
#include <qdebug.h>
#include <qdatetime.h>
#include <QHeaderView>
#include "CalendarEventDialog.h"
#include <QScrollBar>
#include "View/Theme.h"
#include <QPainter>
#include "Presenter/CalendarPresenter.h"
#include "View/uiComponents/CalendarWidget.h"

CalendarView::CalendarView(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    calendarWidget = new CalendarWidget();
    calendarWidget->setWindowFlag(Qt::WindowType::Popup);

    setStyleSheet(Theme::getFancyStylesheet());

    auto arrow = QPixmap(":/icons/icon_downArrow.png");

    ui.prevWeekButton->setIcon(QIcon(QPixmap(arrow.transformed(QTransform().rotate(90)))));
    ui.nextWeekButton->setIcon(QIcon(QPixmap(arrow.transformed(QTransform().rotate(270)))));

    ui.currentWeekButton->setHoverColor(Theme::mainBackgroundColor);
    ui.prevWeekButton->setHoverColor(Theme::mainBackgroundColor);
    ui.nextWeekButton->setHoverColor(Theme::mainBackgroundColor);
    ui.exitButton->setHoverColor(Theme::mainBackgroundColor);
    ui.refreshButton->setHoverColor(Theme::mainBackgroundColor);

    ui.authButton->setIcon(QIcon(":/icons/icon_google.png"));
    ui.exitButton->setIcon(QIcon(":/icons/icon_remove.png"));
    ui.refreshButton->setIcon(QIcon(":/icons/icon_sync.png"));
    
    auto font = ui.calendarButton->font();
    font.setPointSize(font.pointSize() * 2);
    font.setBold(true);
    ui.calendarButton->setFont(font);
    ui.calendarButton->setNormalColor(Theme::background);

    initTable();

    ui.scrollArea->setStyleSheet("#scrollAreaWidgetContents{background-color: white;}");// " +  Theme::colorToString(Theme::background) + ";}");
    ui.weekFrame->setStyleSheet("QFrame{background-color: " + Theme::colorToString(Theme::background) + ";}");
    ui.line->setStyleSheet("color: " + Theme::colorToString(Theme::border) + ";");

    //setting the scrollbar to current time
    auto scrollBar = ui.scrollArea->verticalScrollBar();
    
    QTime time = QTime::currentTime();
    
    int minutes = time.hour() * 60 + time.minute();

    double pixelsPerMin = (double)scrollBar->maximum() / (24 * 60);

    scrollBar->setValue((pixelsPerMin * minutes) - (15*pixelsPerMin));

    connect(ui.authButton, &QPushButton::clicked, this, [=] { presenter->grantAccessRequested(); });
    connect(ui.exitButton, &QPushButton::clicked, this, [=] { presenter->restoreCredentials(); });
    connect(ui.refreshButton, &QPushButton::clicked, this, [=] { presenter->refresh(); });
    connect(ui.nextWeekButton, &QPushButton::clicked, this, [=] { if(presenter) presenter->nextWeekRequested(); });
    connect(ui.prevWeekButton, &QPushButton::clicked, this, [=] { presenter->prevWeekRequested(); });
    connect(ui.currentWeekButton, &QPushButton::clicked, this, [&] { presenter->currentWeekRequested(); });
    connect(ui.calendarCombo, &QComboBox::currentIndexChanged, this, [&](int index) { presenter->calendarIndexChanged(index);});
    connect(ui.calendarTable, &CalendarTable::eventEditRequested, this, [&](int index) { presenter->editEvent(index);});
    connect(ui.calendarTable, &CalendarTable::eventAddRequested, this, [&](const QTime& t, int daysFromMonday, int duration) { presenter->addEvent(t, daysFromMonday, duration); });
    connect(ui.calendarTable, &CalendarTable::deleteEventRequested, this, [&](int eventIdx) { presenter->deleteEvent(eventIdx); });
    connect(ui.calendarTable, &CalendarTable::moveEventRequested, this, [&](int index) { presenter->moveEvent(index); });
    connect(ui.calendarTable, &CalendarTable::operationCanceled, this, [&] { presenter->clearClipboard(); });
    connect(ui.calendarTable, &CalendarTable::eventDurationChange, this, [&](int eventIdx, int duration) { presenter->durationChange(eventIdx, duration); });
    connect(ui.calendarButton, &QPushButton::clicked, this, [&]{ showCalendarWidget(); });
    connect(calendarWidget, &QCalendarWidget::clicked, this, [&](QDate date) { if (presenter)presenter->dateRequested(date); calendarWidget->close();  });
}

void CalendarView::showCalendar(bool show)
{
    ui.stackedWidget->setCurrentIndex(show);
}

void CalendarView::updateWeekView(QDate from, QDate to, int currentDayColumn)
{
    auto calendarWidgetDate = currentDayColumn == -1 ?
        from : from.addDays(currentDayColumn);

    calendarWidget->setSelectedDate(calendarWidgetDate);

    static QString months[] =
    {
        "Януари",
        "Февруари",
        "Март",
        "Април",
        "Май",
        "Юни",
        "Юли",
        "Август",
        "Септември",
        "Октомври",
        "Ноември",
        "Декември"
    };

    int fromMonth = from.month();
    int toMonth = to.month();

    QString label;

    label += QString::number(from.day());

    if (fromMonth != toMonth) {
        label += " ";
        label += months[fromMonth - 1];
        label += " ";
    }

    label += "-";
    
    if (fromMonth != toMonth) {
        label += " ";
    }

    label += QString::number(to.day());
    label += " ";
    label += months[toMonth - 1];
    label += " ";
    label += QString::number(to.year());

    ui.calendarButton->setText(label);

    QLabel* dateLabels[] = {
        ui.labelMon,
        ui.labelTue,
        ui.labelWen,
        ui.labelThu,
        ui.labelFri,
        ui.labelSat,
        ui.labelSun
    };

    QString weekDay[] = {
        "Понеделник","Вторник","Сряда","Четвъртък","Петък","Събота","Неделя"
    };

    QDate date = from;

    auto font = ui.labelMon->font();

    for (int i = 0; i < 7; i++) {
        
        auto& l = dateLabels[i];
        
        QString text = "<p style=\"text-align: center;line-height:150%\"; ><b>";

        text += weekDay[i];

        text += "</b><br>";

        text += date.toString("dd.MM.yyyy г.");

        text += "</p>";

        l->setText(text);
        
        font.setBold(i == currentDayColumn);

        l->setFont(font);
        
        date = date.addDays(1);
    }

    ui.calendarTable->setTodayColumn(currentDayColumn);
}

void CalendarView::initTable()
{
    for (int i = 0; i < 7; i++) {
        ui.calendarTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }

    //(/¯◡ ‿ ◡)/¯ ~ magic, magic 
    int rowHeight = 14;
    int tableHeight = 2303;
    int topOffset = 88;
    int bottomOffset = 16;
    int hourLabelWidth = 60;

    ui.tableTopSpacer->changeSize(20, topOffset);

    for (int i = 0; i < 24*4; i++) {

        ui.calendarTable->setRowHeight(i, rowHeight);
    }

    ui.calendarTable->setMinimumHeight(tableHeight);

    ui.calendarTable->verticalHeader()->hide();
    ui.calendarTable->horizontalHeader()->hide();


    for (int i = 0; i < 23; i++)
    {
        QLabel* l = new QLabel(this);

        auto hour = i + 1;

        QString labelText = "<b><font color=DarkCyan>";// + Theme::colorToString(Theme::fontTurquoiseClicked) + ">";
        labelText += hour < 10 ? "0" : "";
        labelText += QString::number(hour);
        labelText += ":00";
        labelText += "</font></b>";

        l->setText(labelText);
        l->setMinimumWidth(hourLabelWidth);
        l->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
        ui.hourLayout->addWidget(l);
    }
    
    ui.weekSpacerBegin->setMaximumWidth(hourLabelWidth);
    ui.weekSpacerEnd->changeSize(16, 10);

    auto tableBottomSpacer = new QSpacerItem(20, bottomOffset, QSizePolicy::Minimum, QSizePolicy::Fixed);

    ui.hourLayout->addItem(tableBottomSpacer);


}

void CalendarView::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Theme::background);
}

void CalendarView::showCalendarWidget()
{
    auto pos = QCursor::pos();

    calendarWidget->move(pos);

    calendarWidget->show();
}

void CalendarView::setCalendarPresenter(CalendarPresenter* p)
{
    presenter = p;
}

void CalendarView::setCalendarList(const std::vector<Calendar>& list, int currentCalendar)
{
    ui.calendarCombo->blockSignals(true);

    ui.calendarCombo->clear();

    for (auto& c : list) {
        ui.calendarCombo->addItem(c.summary.c_str());
    }

    ui.calendarCombo->setCurrentIndex(currentCalendar);

    ui.calendarCombo->blockSignals(false);
}

void CalendarView::setEventList(const std::vector<CalendarEvent>& list, const CalendarEvent& clipboard_event)
{
    ui.calendarTable->setEvents(list, clipboard_event);
}

CalendarView::~CalendarView()
{
    delete calendarWidget;
}
