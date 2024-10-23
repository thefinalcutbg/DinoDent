#pragma once
#include "Presenter/TabInstance.h"
#include "Model/CalendarStructs.h"
#include <QString>
#include <QDate>

class CalendarView;

class CalendarPresenter : public TabInstance
{
	CalendarView* view;

	std::pair<QDate, QDate> shownWeek;

	std::vector<Calendar> m_calendars;

	std::unordered_map<CalendarCacheKey, std::vector<CalendarEvent>> m_cache;

	int currentCalendar = 0;

	std::string deleteId;

	CalendarEvent clipboard_event;

	enum QueryType { GetCalendars, GetEvents, UploadEvent, DeleteEvent };

	static std::pair<QDate, QDate> getTodaysWeek();

	void requestEvents(bool searchCache = true);

	void setClipboard(const CalendarEvent& e);

	void sendEventQuery(const CalendarEvent& event);

	int getCurrentDayColumn();

	CalendarCacheKey getCacheKey() const;

	const std::vector<CalendarEvent>& getEvents() const;

public:

	CalendarPresenter(ITabView* view);

	// Inherited via TabInstance
	void setDataToView() override {};
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
	void addEvent(const QTime& t, int daysFromMonday, int duration);
	void editEvent(int index);
	void deleteEvent(int index);
	void clearClipboard();
	void durationChange(int eventIdx, int duration);
	void refresh();
	void cancelMove();

	~CalendarPresenter();
};