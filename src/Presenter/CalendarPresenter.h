#pragma once
#include "Presenter/TabInstance.h"
#include "Model/CalendarStructs.h"
#include <QString>
#include <QDate>

class CalendarView;


struct CalendarCacheKey {
	int calendarIdx;
	int year;
	int weekNumber;

	bool operator==(const CalendarCacheKey& other) const
	{
		return (calendarIdx == other.calendarIdx
			&& year == other.year
			&& weekNumber == other.weekNumber);
	}
};

template <>
struct std::hash<CalendarCacheKey>
{
	std::size_t operator() (const CalendarCacheKey& node) const
	{
		std::size_t h1 = std::hash<int>()(node.calendarIdx);
		std::size_t h2 = std::hash<int>()(node.year);
		std::size_t h3 = std::hash<int>()(node.weekNumber);
		return h1 ^ h2 ^ h3;
	}
};

class CalendarPresenter : public TabInstance
{
	CalendarView* view;

	std::pair<QDate, QDate> shownWeek;

	std::vector<Calendar> m_calendars;

	std::unordered_map<CalendarCacheKey, std::vector<CalendarEvent>> m_cache;

	bool awaitingQuery = false;

	int currentCalendar = -1;

	std::string deleteId;

	CalendarEvent clipboard_event;

	enum QueryType { GetCalendars, GetEvents, UploadEvent, DeleteEvent, NoReply=99 };

	static std::pair<QDate, QDate> getTodaysWeek();

	void requestEvents(bool searchCache = true);

	void sendEventQuery(const CalendarEvent& event);

	int getCurrentDayColumn();

	CalendarCacheKey getCacheKey() const;

	void setClipboard(const CalendarEvent& e);

	std::vector<CalendarEvent>* getEvents();
	CalendarEvent* getEvent(int eventIndex);

public:

	CalendarPresenter(ITabView* view);
	void newAppointment(const CalendarEvent& event);

	// Inherited via TabInstance
	void setDataToView() override;
	bool isNew() override { return false; }
	TabName getTabName() override;
	bool save() override { return true; }
	void print() override {}
	long long rowID() const override { return 0; }

	//called by OAuth
	void disconnected();
	void restoreCredentials();
	void authorizationSuccessful(const std::string& refreshToken = std::string()); //if token is empty, then authentification has failed
	void setReply(const std::string& reply, int callbackIdx);

	//called from view:
	void grantAccessRequested();
	void calendarIndexChanged(int idx);
	void nextWeekRequested();
	void prevWeekRequested();
	void dateRequested(QDate date);
	void currentWeekRequested();
	void moveEvent(int index);
	void newDocRequested(int index, TabType type);
	void addEvent(const QTime& t, int daysFromMonday, int duration);
	void editEvent(int index);
	void deleteEvent(int index);
	void clearClipboard();
	void durationChange(int eventIdx, int duration);
	void refresh();
	void cancelMove();

	~CalendarPresenter();
};