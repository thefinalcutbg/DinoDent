#include "customdateedit.h"

CustomDateEdit::CustomDateEdit(QWidget * parent, menuContent type) :
    QDateEdit (parent) {
    this->type = type;
    initCalendar();
}
CustomDateEdit::~CustomDateEdit() {}
/* 初始化日历菜单 */
void CustomDateEdit::initCalendar() {
    setDate(QDate::currentDate());
    setCalendarPopup(true); // 设置日期编辑为弹出式
    calendarWidget()->setLocale(QLocale(QLocale::Chinese)); // 设置程序的区域为中文使用地区
    calendarWidget()->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader); // 去除日历左边的列头
    calendarWidget()->setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames); // 只显示周的一个字
    calendarWidget()->setFirstDayOfWeek(Qt::Monday); // 设置每周由周一开始
    calendarWidget()->setNavigationBarVisible(false);
    QTextCharFormat dateCharFormat; // 日期文本样式格式
    dateCharFormat.setBackground(Qt::white); // 背景色
    QTextCharFormat headerCharFormat;
    headerCharFormat.setBackground(Qt::white);
    headerCharFormat.setForeground(Qt::gray);
    calendarWidget()->setHeaderTextFormat(headerCharFormat); // 设置"周几"文本的样式
    /* 星期(周)的每一天的文本样式 */
    for (int dayOfWeek = Qt::Monday; dayOfWeek <= Qt::Sunday; dayOfWeek++) {
        calendarWidget()->setWeekdayTextFormat(Qt::DayOfWeek(dayOfWeek), dateCharFormat);
    }
    QFile dateEditFile(":/style/dateEditStyle.qss"); // 取样式文件
    if (dateEditFile.open(QFile::ReadOnly)) { // 文件可以只读方式打开
        QString styleSheet = QLatin1String(dateEditFile.readAll()); // 获取样式文件内容
        setStyleSheet(styleSheet); // 设置样式表
        dateEditFile.close(); // 关闭文件
    }
    initHeaderWidget(); // 初始化日期导航条
}
/* 初始化日期导航条 */
void CustomDateEdit::initHeaderWidget() {
    QWidget * topWidget = new QWidget(calendarWidget());
    topWidget->setObjectName("headerWidget");
    topWidget->setFixedHeight(40);
    topWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout * hboxLayout = new QHBoxLayout(this);
    hboxLayout->setContentsMargins(12, 0, 12, 0);
    hboxLayout->setSpacing(4);

    prevYearButton = new QPushButton(topWidget); // 上一年按钮
    prevYearButton->setObjectName("prevYearButton");
    prevYearButton->setFixedSize(16, 16);
    prevYearButton->setToolTip("上一年");

    nextYearButton = new QPushButton(topWidget); // 下一年按钮
    nextYearButton->setObjectName("nextYearButton");
    nextYearButton->setFixedSize(16, 16);
    nextYearButton->setToolTip("下一年");

    prevMonthButton = new QPushButton(topWidget); // 上个月按钮
    prevMonthButton->setObjectName("prevMonthButton");
    prevMonthButton->setFixedSize(16, 16);
    prevMonthButton->setToolTip("上一个月");

    nextMonthButton = new QPushButton(topWidget); // 下个月按钮
    nextMonthButton->setObjectName("nextMonthButton");
    nextMonthButton->setFixedSize(16, 16);
    nextMonthButton->setToolTip("下一个月");

    yearButton = new QPushButton(topWidget); // 年份按钮
    yearButton->setObjectName("yearButton");

    monthButton = new QPushButton(topWidget); // 月份按钮
    monthButton->setObjectName("monthButton");

    setDateLabelText(QDate::currentDate().year(), QDate::currentDate().month()); // 设置初始的日期文本

    hboxLayout->addWidget(prevYearButton);
    hboxLayout->addWidget(prevMonthButton);
    hboxLayout->addStretch();
    hboxLayout->addWidget(yearButton);
    hboxLayout->addWidget(monthButton);
    hboxLayout->addStretch();
    hboxLayout->addWidget(nextMonthButton);
    hboxLayout->addWidget(nextYearButton);
    topWidget->setLayout(hboxLayout);

    vBodyLayout = qobject_cast<QVBoxLayout *>(calendarWidget()->layout()); // 获取日历控件的布局
    vBodyLayout->insertWidget(0, topWidget);

    monthMenu = new QWidget(calendarWidget()); // 月份菜单
    monthMenu->setObjectName("monthMenu");
    vBodyLayout->insertWidget(1, monthMenu);
    monthMenu->hide();
    monthGridLayout = new QGridLayout(this); // 月份菜单的栅格布局
    monthMenu->setLayout(monthGridLayout);
    monthGridLayout->setSpacing(0);
    monthMenu->installEventFilter(this); // 月份菜单注册鼠标滚轮过滤事件

    yearMenu = new QWidget(calendarWidget()); // 年份菜单
    yearMenu->setObjectName("yearMenu");
    vBodyLayout->insertWidget(2, yearMenu);
    yearMenu->hide();
    yearGridLayout = new QGridLayout(this); // 年份菜单的栅格布局
    yearMenu->setLayout(yearGridLayout);
    yearGridLayout->setSpacing(0);
    yearMenu->installEventFilter(this); // 年份菜单注册鼠标滚轮过滤事件

    changeType(type);

    connect(prevYearButton, &QPushButton::clicked, [this](){
        selectedYear(-1);
    }); // 上一年按钮的信号槽
    connect(nextYearButton, &QPushButton::clicked, [this](){
        selectedYear(1);
    }); // 下一年按钮的信号槽
    connect(prevMonthButton, &QPushButton::clicked, [this](){
        int year = calendarWidget()->selectedDate().year();
        int month = calendarWidget()->selectedDate().month();
        selectedMonth(year, month - 1);
    }); // 上一月按钮的信号槽
    connect(nextMonthButton, &QPushButton::clicked, [this](){
        int year = calendarWidget()->selectedDate().year();
        int month = calendarWidget()->selectedDate().month();
        selectedMonth(year, month + 1);
    }); // 下一月按钮的信号槽
    connect(calendarWidget(), &QCalendarWidget::currentPageChanged, [this](int year, int month) {
        setDateLabelText(year, month);
        selectedMonth(year, month);
        menuChanged();
    }); // 当日期改变时设置日期标签的文本和作用到日期并更新日历菜单内容
    connect(monthButton, &QPushButton::clicked, [this]() {
        if (monthMenu->isHidden()) {
            monthMenuPopup();
        } else {
            changeMenu(type);
        }
    }); // 月份按钮的信号槽
    connect(yearButton, &QPushButton::clicked, [this]() {
        if (yearMenu->isHidden()) {
            yearMenuPopup();
        } else {
            changeMenu(type);
        }
    }); // 年份按钮的信号槽
}
/* 设置日期标签的文本 */
void CustomDateEdit::setDateLabelText(int year, int month) {
    yearButton->setText(QStringLiteral("%1年").arg(year));
    monthButton->setText(QStringLiteral("%1月").arg(month));
}
/* 选择月份 */
void CustomDateEdit::selectedMonth(int year, int month) {
    if (year >= minimumDate().year() && year <= maximumDate().year()) {
        QDate seletedDate = calendarWidget()->selectedDate(); // 当前选中的月份
        QDate validDate = seletedDate.addMonths(month - seletedDate.month()); // 按月份加减时间,返回有效时间
        setDate(validDate);
    }
}
/* 选择年份 */
void CustomDateEdit::selectedYear(int yearCount) {
    QDate validDate = calendarWidget()->selectedDate().addYears(yearCount); // 按年份加减时间,返回有效日期
    setDate(validDate);
}
/* 展示月份菜单 */
void CustomDateEdit::monthMenuPopup() {
    changeMenu(menuContent::MONTH);
    QList<QPushButton *> monthList;
    int currentMonth = calendarWidget()->selectedDate().month(); // 当前月份
    int initializationMonth = currentMonth - 12; // 月份菜单中的初始月份
    int minMonth = 0 - initializationMonth; // 在月份菜单中当前年份可显示的最小月份
    int maxMonth = 13 + minMonth; // 在月份菜单中当前年份可显示的最大月份
    int monthTableCount = 0; // 用于月份菜单的数组下标
    int monthCount = initializationMonth; // 用于选择时的月份加减时间
    int actualMonth = monthCount; // 实际显示的月份
    for (int monthTableRow = 0; monthTableRow < 4; monthTableRow++) { // 月份菜单的行
        for (int monthTableColumn = 0; monthTableColumn < 4; monthTableColumn++) { // 月份菜单的列
            monthList << new QPushButton(this);
            actualMonth = (monthCount <= 0)
                    ? (monthCount + 12) : ((monthCount) > 12)
                      ? (monthCount % 12) : (monthCount);
            monthList[monthTableCount]->setText(QStringLiteral("%1月").arg(actualMonth));
            if (monthTableCount == 12) {
                monthList[monthTableCount]->setObjectName("monthListSelected");
            } else if (monthTableCount > minMonth && monthTableCount < maxMonth) {
                monthList[monthTableCount]->setObjectName("monthListEnabled");
            } else if (monthTableCount <= minMonth || monthTableCount >= maxMonth) {
                monthList[monthTableCount]->setObjectName("monthListDisabled");
            }
            monthGridLayout->addWidget(monthList[monthTableCount], monthTableRow, monthTableColumn);
            connect(monthList[monthTableCount], &QPushButton::clicked, [=]() {
                int year = calendarWidget()->selectedDate().year();
                selectedMonth(year, monthCount);
                if (type == menuContent::MONTH) {
                    calendarWidget()->activated(calendarWidget()->selectedDate());
                } else {
                    changeMenu(menuContent::DAY);
                }
            });
            monthCount++;
            monthTableCount++;
        }
    }
}
/* 展示年份菜单 */
void CustomDateEdit::yearMenuPopup() {
    changeMenu(menuContent::YEAR);
    QList<QPushButton *> yearList;
    int currentYear = calendarWidget()->selectedDate().year(); // 当前年份
    int initializationYear = currentYear - 12; // 年份菜单中显示的初始年份
    int yearTableCount = 0; // 用于年份菜单的数组下标
    int yearCount = initializationYear; // 年份菜单中实际展示的年份和用于加减的年份
    for (int yearTableRow = 0; yearTableRow < 4; yearTableRow++) { // 年份的行
        for (int yearTableColumn = 0; yearTableColumn < 4; yearTableColumn++) { // 年份的列
            yearList << new QPushButton(this);
            yearList[yearTableCount]->setText(QStringLiteral("%1年").arg(yearCount));
            if (yearTableCount == 12) {
                yearList[yearTableCount]->setObjectName("yearListSelected");
            } else if (yearTableCount <= 2 || yearTableCount >= 13) {
                yearList[yearTableCount]->setObjectName("yearListDisabled");
            } else if (yearTableCount > 2 && yearTableCount < 13) {
                yearList[yearTableCount]->setObjectName("yearListEnabled");
            }
            yearGridLayout->addWidget(yearList[yearTableCount], yearTableRow, yearTableColumn);
            connect(yearList[yearTableCount], &QPushButton::clicked, [=]() {
                int currentYear = calendarWidget()->selectedDate().year();
                selectedYear(yearCount - currentYear);
                if (type == menuContent::YEAR) {
                    calendarWidget()->activated(calendarWidget()->selectedDate());
                } else {
                    changeMenu(type);
                }
            });
            yearCount++;
            yearTableCount++;
        }
    }
}
/* 菜单发生变化时展示菜单的新内容 */
void CustomDateEdit::menuChanged() {
    if (!yearMenu->isHidden()) {
        yearMenuPopup();
    } else if (!monthMenu->isHidden()) {
        monthMenuPopup();
    }
}
/* 切换菜单 */
void CustomDateEdit::changeMenu(int sequence) {
    vBodyLayout->itemAt(4)->widget()->hide(); // 隐藏日期菜单
    yearMenu->hide(); // 隐藏年份菜单
    monthMenu->hide(); // 隐藏月份菜单
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
/* 过滤鼠标滚轮事件 */
bool CustomDateEdit::eventFilter(QObject *widget, QEvent *event) {
    if (widget == yearMenu || widget == monthMenu) {
        if (event->type() == QEvent::Wheel) {
            QWheelEvent * wheelEvent = static_cast<QWheelEvent *>(event);
            int angelDeltaY = wheelEvent->angleDelta().y();
            int year = calendarWidget()->selectedDate().year();
            int month = calendarWidget()->selectedDate().month();
            if (widget == yearMenu && angelDeltaY > 0) { // 在年份菜单上向上滚动
                selectedYear(-4);
            } else if (widget == yearMenu && angelDeltaY < 0) { // 在年份菜单上向下滚动
                selectedYear(4);
            } else if (widget == monthMenu && angelDeltaY > 0) { // 在月份菜单上向上滚动
                selectedMonth(year, month - 4);
            } else if (widget == monthMenu && angelDeltaY < 0) { // 在月份菜单上向下滚动
                selectedMonth(year, month + 4);
            }
            menuChanged();
        }
    }
    return true;
}
/* 改变日历菜单类型 */
void CustomDateEdit::changeType(menuContent type) {
    this->type = type;
    if (type == menuContent::DAY) {
        changeMenu(menuContent::DAY);
        setDisplayFormat("yyyy-MM-dd");
        monthButton->show();
    } else if (type == menuContent::MONTH) {
        monthMenuPopup();
        setDisplayFormat("yyyy-MM");
        monthButton->show();
    } else if (type == menuContent::YEAR) {
        yearMenuPopup();
        setDisplayFormat("yyyy");
        monthButton->hide();
    }
}
