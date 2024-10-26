#pragma once

#include <QTableView>
#include <QAbstractItemModel>
#include <QStyledItemDelegate>
#include "View/Graphics/CalendarViewData.h"
#include <utility>
#include "Model/TabType.h"

class CalendarTable;

class EventDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	CalendarViewData& data;
	CalendarTable* view{ nullptr };

	QString requestTime(int row) const;

public:
	std::pair<int, int> emptyHovered{ -1, -1 };

	EventDelegate(CalendarTable* view, CalendarViewData& data);
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);

	signals:
		void clicked(int column, int row);
		
};

//placeholder model - it doesn't use anything. Everything is painted from scratch by event delegate
class CalendarTableModel : public QAbstractTableModel
{
	Q_OBJECT

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override { return QVariant(); };

public:
	CalendarTableModel() {};

	int rowCount(const QModelIndex& parent = QModelIndex()) const override { return 96; }
	int columnCount(const QModelIndex& parent = QModelIndex()) const override { return 7; }

	~CalendarTableModel() {};

};

class CalendarTable : public QTableView
{
	Q_OBJECT

	CalendarTableModel m_model;

	CalendarViewData m_data;

	EventDelegate* delegate_ptr = nullptr;

	int m_today_column = -1;

	void leaveEvent(QEvent* event) override;

	QMenu* context_menu{ nullptr };

	bool menu_click_guard = false;

	void menuRequested(int column, int row);

	void paintEvent(QPaintEvent*) override;

public:
	CalendarTable(QWidget* parent = nullptr);

	void cellClicked(int column, int row, bool leftClick);

	void setEvents(const std::vector<CalendarEvent>& list, const CalendarEvent& clipboardEvent = CalendarEvent());

	void setTodayColumn(int today);

	int todayColumn() const { return m_today_column; }

signals:

	void newDocRequested(int eventIndex, TabType type);
	void eventEditRequested(int eventIndex);
	void moveEventRequested(int eventIndex);
	void deleteEventRequested(int eventIndex);
	void eventAddRequested(const QTime& t, int daysFromMonday, int minDuration);
	void eventDurationChange(int eventIndx, int minDuration);
	void operationCanceled();

};