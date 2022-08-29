#ifndef CUSTOMDATEEDIT_H
#define CUSTOMDATEEDIT_H

#include <QDateEdit>
#include <QMenu>
#include <QFile>
#include <QTextCharFormat>
#include <QCalendarWidget>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QWheelEvent>

class CustomDateEdit : public QDateEdit
{
    Q_OBJECT

public:
    enum menuContent {YEAR, MONTH, DAY}; // 表示菜单种类的枚举值
    explicit CustomDateEdit(QWidget * parent = nullptr, menuContent type = menuContent::DAY);
    ~CustomDateEdit();
    void changeType(menuContent type);

private slots:
    void selectedMonth(int year, int month);    // 选择月份的槽函数
    void setDateLabelText(int year, int month);
    void selectedYear(int yearCount);           // 选择年份的槽函数
    void monthMenuPopup();                      // 展示月份菜单的槽函数
    void yearMenuPopup();                       // 展示年份菜单的槽函数
    void menuChanged();                         // 菜单内容改变的槽函数
    void changeMenu(int sequence);              // 切换菜单的槽函数

private:
    void initCalendar();            // 初始化整个日历菜单
    void initHeaderWidget();        // 初始化日历导航条
    menuContent type;               // 日历菜单类型
    QPushButton * prevYearButton;   // 上一年按钮
    QPushButton * nextYearButton;   // 下一年按钮
    QPushButton * prevMonthButton;  // 上一月按钮
    QPushButton * nextMonthButton;  // 下一月按钮
    QPushButton * yearButton;       // 年份按钮
    QPushButton * monthButton;      // 月份按钮
    QVBoxLayout * vBodyLayout;      // 获取的父类日历控件的布局
    QGridLayout * monthGridLayout;  // 月份菜单的栅格布局
    QGridLayout * yearGridLayout;   // 年份菜单的栅格布局
    QWidget * monthMenu;            // 月份菜单
    QWidget * yearMenu;             // 年份菜单

protected:
    bool eventFilter(QObject * widget, QEvent * event); // 鼠标滚轮的过滤事件
};

#endif // CUSTOMDATEEDIT_H
