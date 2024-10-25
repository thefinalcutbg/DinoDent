#include "CalendarPresenter.h"
#include "View/Widgets/CalendarView.h"
#include "Database/DbDoctor.h"
#include "Model/User.h"
#include "View/Widgets/CalendarView.h"
#include "Network/Calendar/Google.h"
#include "Network/Calendar/CalendarJsonParser.h"
#include "View/Widgets/CalendarEventDialog.h"

CalendarPresenter::CalendarPresenter(ITabView* tabView) :
    TabInstance(tabView, TabType::Calendar, nullptr),
    view(tabView->calendarView())
{
    view->showCalendar(false);

    Google::setReciever(this);

    view->setCalendarPresenter(this);

    shownWeek = getTodaysWeek();

    view->updateWeekView(shownWeek.first, shownWeek.second, getCurrentDayColumn());

    auto refreshToken = DbDoctor::calendarRefreshToken(User::doctor().LPK);

    if (refreshToken.empty()) {
        return;
    }

    Google::grantAccess(refreshToken);
}

void CalendarPresenter::newAppointment(const std::string& eventName)
{
    clipboard_event = CalendarEvent();
    clipboard_event.summary = eventName;

    //the data is already downloaded
    if (currentCalendar != -1) {

        auto e = getEvents();

        if (e) {
            view->setEventList(*e, clipboard_event);
        }
    }
}

void CalendarPresenter::grantAccessRequested()
{
    auto refreshToken = DbDoctor::calendarRefreshToken(User::doctor().LPK);

    Google::grantAccess(refreshToken);
}

TabName CalendarPresenter::getTabName()
{
    return TabName{
        .header = "График на",
        .footer = "посещенията",
        .header_icon = CommonIcon::CALENDAR
        
    };
}

void CalendarPresenter::disconnected()
{
    view->showCalendar(false);
}

void CalendarPresenter::restoreCredentials()
{
    DbDoctor::setCalendarRefreshToken("", User::doctor().LPK);
    DbDoctor::setCurrentCalendarIdx(0, User::doctor().LPK);
    view->showCalendar(false);
}

void CalendarPresenter::authorizationSuccessful(const std::string& refreshToken)
{
    view->showCalendar(true);

    DbDoctor::setCalendarRefreshToken(refreshToken, User::doctor().LPK);

    currentCalendar = DbDoctor::currentCalendarIdx(User::doctor().LPK);

    view->updateWeekView(shownWeek.first, shownWeek.second, getCurrentDayColumn());
    
    Google::query(
        "https://www.googleapis.com/calendar/v3/users/me/calendarList",
        {}, "GET", {}, QueryType::GetCalendars
    );
}

void CalendarPresenter::setReply(const std::string& reply, int callbackIdx)
{
    awaitingQuery = false;

    auto key = getCacheKey();

    switch (callbackIdx)
    {
    case QueryType::GetCalendars:
    {
        m_calendars = CalendarJsonParser::parseCalendarList(reply);

        view->setCalendarList(m_calendars, currentCalendar);

        requestEvents();
    }
        break;

    case QueryType::GetEvents:

        m_cache[key] = CalendarJsonParser::parseEventList(reply);
        
        view->setEventList(m_cache[key], clipboard_event);

        break;

    case QueryType::UploadEvent:
    {
        auto event = CalendarJsonParser::parseEvent(reply);

        bool eventExists = false;

        for (auto& e : m_cache[key]) {

            if (e.id == event.id) {
                e = event;
                eventExists = true;
                break;
            }
        }

        if (!eventExists && event.start.date().weekNumber() == shownWeek.first.weekNumber()) {

            m_cache[key].push_back(event);
        }

        if (
            clipboard_event.summary.size() &&
            clipboard_event.summary == event.summary
        ) {
            clipboard_event = CalendarEvent();
        }

        view->setEventList(m_cache[key], clipboard_event);
    }

    break;

    case QueryType::DeleteEvent:
    {
        std::string deleteIdTemp = deleteId;
        deleteId.clear();

        if (reply.size()) return; //empty response means success. otherwise - failure

        auto& m_events = m_cache[key];

        for (int i = 0; i < m_events.size(); i++) {

            if (m_events[i].id == deleteIdTemp) {

                m_events.erase(m_events.begin() + i);

                view->setEventList(m_events, clipboard_event);

                return;
            }
        }
    }
        break;

    case QueryType::NoReply:
        ModalDialogBuilder::showMessage("Неуспешна връзка с Google Calendar");
        return;
    }
}

void CalendarPresenter::calendarIndexChanged(int idx)
{
    DbDoctor::setCurrentCalendarIdx(idx, User::doctor().LPK);
    currentCalendar = idx;

    requestEvents();
}

void CalendarPresenter::nextWeekRequested()
{
    if (awaitingQuery) return;

    shownWeek.first = shownWeek.first.addDays(7);
    shownWeek.second = shownWeek.second.addDays(7);

    requestEvents();
}

void CalendarPresenter::prevWeekRequested()
{
    if (awaitingQuery) return;

    shownWeek.first = shownWeek.first.addDays(-7);
    shownWeek.second = shownWeek.second.addDays(-7);

    requestEvents();
}

void CalendarPresenter::dateRequested(QDate date)
{
    if (awaitingQuery) return;

    if (date.year() == shownWeek.first.year() &&
        date.weekNumber() == shownWeek.first.weekNumber()) {

        return;
    }

    shownWeek.first = date.addDays(-date.dayOfWeek() + 1);
    shownWeek.second = date.addDays(-(date.dayOfWeek() - 7));

    requestEvents();
}


void CalendarPresenter::refresh()
{
    disconnected();
    view->setEventList({}, clipboard_event);
    m_cache.clear();
    m_calendars.clear();
    currentCalendar = -1;
    grantAccessRequested();
}


void CalendarPresenter::currentWeekRequested()
{
    if (awaitingQuery) return;

    auto todaysWeek = getTodaysWeek();

    if (todaysWeek == shownWeek) return;

    shownWeek = todaysWeek;

    requestEvents();
}

void CalendarPresenter::moveEvent(int index)
{
    auto events = getEvents();

    if (!events) return;

    if (index < 0 || index >= events->size()) return;

    setClipboard(events->at(index));
}

void CalendarPresenter::addEvent(const QTime& t, int daysFromMonday, int duration)
{
    if (awaitingQuery) return;

    QDateTime from(shownWeek.first.addDays(daysFromMonday), t);
    QDateTime to(from.addSecs(duration * 60));

    clipboard_event.start = from;
    clipboard_event.end = to;

    if (clipboard_event.id.size() || clipboard_event.summary.size()) { //existing event
        sendEventQuery(clipboard_event);
        return;
    }

    CalendarEventDialog d(clipboard_event);

    if (d.exec() != QDialog::Accepted) return;

    sendEventQuery(d.result());
}

void CalendarPresenter::editEvent(int index)
{
    auto e = getEvents();

    if (awaitingQuery || !e) return;

    if (index < 0 || index >= e->size()) return;

    CalendarEventDialog d(e->at(index));

    if (d.exec() != QDialog::Accepted) return;

    sendEventQuery(d.result());
}

void CalendarPresenter::deleteEvent(int index)
{
    if (awaitingQuery) return;

    auto events = getEvents();

    if (!events) return;

    if (index < 0 || index >= events->size()) return;

    deleteId = events->at(index).id;

    QVariantMap parameters;
    parameters["calendarId"] = m_calendars[currentCalendar].id.c_str();
    parameters["eventId"] = events->at(index).id.c_str();

    Google::query(
        "https://www.googleapis.com/calendar/v3/calendars/calendarId/events/eventId",
        parameters, "DELETE", {}, QueryType::DeleteEvent
    );
}

void CalendarPresenter::clearClipboard()
{
    clipboard_event = CalendarEvent{};

    auto e = getEvents();

    view->setEventList(*e, clipboard_event);
}

void CalendarPresenter::durationChange(int eventIdx, int duration)
{
    if (awaitingQuery) return;

    auto e = getEvents();

    if (!e) return;

    auto event = e->at(eventIdx);

    event.end = event.start.addSecs(duration * 60);

    sendEventQuery(event);
}

void CalendarPresenter::cancelMove()
{
    clearClipboard();
}

CalendarPresenter::~CalendarPresenter()
{
    Google::setReciever(nullptr);
}

std::pair<QDate, QDate> CalendarPresenter::getTodaysWeek()
{
    auto currentDate = QDate::currentDate();
    int dayOfWeek = currentDate.dayOfWeek();
    return std::make_pair(currentDate.addDays(1 - dayOfWeek), currentDate.addDays(7 - dayOfWeek));
}

void CalendarPresenter::requestEvents(bool searchCache)
{
    view->updateWeekView(shownWeek.first, shownWeek.second, getCurrentDayColumn());

    auto key = getCacheKey();

    if (m_cache.count(key) && searchCache) {
        view->setEventList(m_cache[key], clipboard_event);
        return;
    }

    awaitingQuery = true;

    view->setEventList({}, clipboard_event);

    auto& calendar = m_calendars[currentCalendar];

    QVariantMap param;
    param["calendarId"] = QString(calendar.id.c_str());
    param["timeMin"] = shownWeek.first.toString("yyyy-MM-dd") + "T00:00:00Z";
    param["timeMax"] = shownWeek.second.toString("yyyy-MM-dd") + "T23:59:59Z";
    param["timeZone"] = "Europe/Sofia";

    Google::query(
        "https://www.googleapis.com/calendar/v3/calendars/calendarId/events",
        param, "GET", {}, QueryType::GetEvents
    );
}

void CalendarPresenter::setClipboard(const CalendarEvent& e)
{
    clipboard_event = e;

    auto& events = m_cache[getCacheKey()];

    std::sort(events.begin(), events.end(),
        [](const CalendarEvent& left, const CalendarEvent& right)
        {
            return left.start < right.start;
        }
    );

    view->setEventList(events, clipboard_event);
}

void CalendarPresenter::sendEventQuery(const CalendarEvent& event)
{
    awaitingQuery = true;

    //edit
    if (event.id.size()) {

        QVariantMap parameters;
        parameters["calendarId"] = m_calendars[currentCalendar].id.c_str();
        parameters["eventId"] = event.id.c_str();

        Google::query(
            "https://www.googleapis.com/calendar/v3/calendars/calendarId/events/eventId",
            parameters, "PUT", CalendarJsonParser::writeEventQuery(event).data(), QueryType::UploadEvent
        );

        return;
    }

    //add new
    QVariantMap parameters;
    parameters["calendarId"] = m_calendars[currentCalendar].id.c_str();

    Google::query(
        "https://www.googleapis.com/calendar/v3/calendars/calendarId/events",
        parameters, "POST", CalendarJsonParser::writeEventQuery(event).data(), QueryType::UploadEvent
    );
}

int CalendarPresenter::getCurrentDayColumn()
{
    QDate currentDate = QDate::currentDate();

    if (currentDate >= shownWeek.first && currentDate <= shownWeek.second) {
        return currentDate.dayOfWeek() - 1;
    }

    return -1;
}

CalendarCacheKey CalendarPresenter::getCacheKey() const
{
    return CalendarCacheKey{
    .calendarIdx = currentCalendar,
    .year = shownWeek.first.year(),
    .weekNumber = shownWeek.first.weekNumber()
    };
}

std::vector<CalendarEvent>* CalendarPresenter::getEvents()
{

    CalendarCacheKey key{
        .calendarIdx = currentCalendar,
        .year = shownWeek.first.year(),
        .weekNumber = shownWeek.first.weekNumber()
    };

    return m_cache.count(key) ? &m_cache.at(key) : nullptr;
}
