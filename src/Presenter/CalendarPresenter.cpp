#include "CalendarPresenter.h"
#include "View/Widgets/CalendarView.h"
#include "Database/DbDoctor.h"
#include "Model/User.h"
#include "View/Widgets/CalendarView.h"
#include "Network/Calendar/Google.h"
#include "Network/Calendar/CalendarJsonParser.h"
#include "View/Widgets/CalendarEventDialog.h"

CalendarPresenter::CalendarPresenter(ITabView* view) :
    TabInstance(view, TabType::Calendar, nullptr),
    view(view->calendarView()),
    currentCalendar(DbDoctor::currentCalendarIdx(User::doctor().LPK))
{
    Google::setReciever(this);

    this->view->setCalendarPresenter(this);

    auto refreshToken = DbDoctor::calendarRefreshToken(User::doctor().LPK);

    if (refreshToken.empty()) {
        this->view->showCalendar(false);
        return;
    }

    Google::grantAccess(refreshToken);

    shownWeek = getTodaysWeek();
}

TabName CalendarPresenter::getTabName()
{
    return TabName{
        .header = "График на посещенията",
        .header_icon = CommonIcon::CALENDAR
    };
}

void CalendarPresenter::authorizationSuccessful(const std::string& refreshToken)
{
    if (!refreshToken.size()) return;

    DbDoctor::setCalendarRefreshToken(refreshToken, User::doctor().LPK);
    
    this->view->showCalendar(true);
    view->updateWeekView(shownWeek.first, shownWeek.second, getCurrentDayColumn());
    
    Google::query(
        "https://www.googleapis.com/calendar/v3/users/me/calendarList",
        {}, "GET", {}, QueryType::GetCalendars
    );
}

void CalendarPresenter::setReply(const std::string& reply, int callbackIdx)
{
    switch (callbackIdx)
    {
    case QueryType::GetCalendars:
    {

        m_calendars = CalendarJsonParser::parseCalendarList(reply);

        auto idx = DbDoctor::currentCalendarIdx(User::doctor().LPK);

        view->setCalendarList(m_calendars, idx);

        requestEvents();;
    }
        break;

    case QueryType::GetEvents:

        m_events = CalendarJsonParser::parseEventList(reply);

        view->setEventList(m_events, clipboard_event);

        break;

    case QueryType::UploadEvent:
    {
        auto event = CalendarJsonParser::parseEvent(reply);

        bool eventExists = false;

        for (auto& e : m_events) {

            if (e.id == event.id) {
                e = event;
                eventExists = true;
                break;
            }
        }

        if (!eventExists && event.start.date().weekNumber() == shownWeek.first.weekNumber()) {

            m_events.push_back(event);
        }

        clipboard_event = CalendarEvent();

        view->setEventList(m_events, clipboard_event);
    }

    break;

    case QueryType::DeleteEvent:
    {
        std::string deleteIdTemp = deleteId;
        deleteId.clear();

        if (reply.size()) return; //empty response means success. otherwise - failure

        for (int i = 0; i < m_events.size(); i++) {

            if (m_events[i].id == deleteIdTemp) {

                m_events.erase(m_events.begin() + i);

                view->setEventList(m_events, clipboard_event);



                return;
            }
        }
    }
        break;
    }
}

void CalendarPresenter::grantAccessRequested()
{
    Google::grantAccess();
}

void CalendarPresenter::calendarIndexChanged(int idx)
{
    DbDoctor::setCurrentCalendarIdx(idx, User::doctor().LPK);
    currentCalendar = idx;

    requestEvents();
}

void CalendarPresenter::nextWeekRequested()
{
    view->setEventList({}, clipboard_event);

    shownWeek.first = shownWeek.first.addDays(7);
    shownWeek.second = shownWeek.second.addDays(7);

    view->updateWeekView(shownWeek.first, shownWeek.second, getCurrentDayColumn());

    requestEvents();
}

void CalendarPresenter::prevWeekRequested()
{
    view->setEventList({}, clipboard_event);

    shownWeek.first = shownWeek.first.addDays(-7);
    shownWeek.second = shownWeek.second.addDays(-7);

    view->updateWeekView(shownWeek.first, shownWeek.second, getCurrentDayColumn());

    requestEvents();
}

void CalendarPresenter::currentWeekRequested()
{
    view->setEventList({}, clipboard_event);

    auto todaysWeek = getTodaysWeek();

    if (todaysWeek == shownWeek) return;

    shownWeek = getTodaysWeek();

    view->updateWeekView(shownWeek.first, shownWeek.second, getCurrentDayColumn());

    requestEvents();
}

void CalendarPresenter::moveEvent(int index)
{
    if (index < 0 || index >= m_events.size()) return;

    setClipboard(m_events[index]);
}

void CalendarPresenter::addEvent(const QTime& t, int daysFromMonday, int duration)
{
    QDateTime from(shownWeek.first.addDays(daysFromMonday), t);
    QDateTime to(from.addSecs(duration * 60));

    clipboard_event.start = from;
    clipboard_event.end = to;

    if (clipboard_event.id.size()) { //existing event
        sendEventQuery(clipboard_event);
        return;
    }

    CalendarEventDialog d(clipboard_event);

    if (d.exec() != QDialog::Accepted) return;

    sendEventQuery(d.result());
}

void CalendarPresenter::editEvent(int index)
{
    CalendarEventDialog d(m_events[index]);

    if (d.exec() != QDialog::Accepted) return;

    sendEventQuery(d.result());
}

void CalendarPresenter::deleteEvent(int index)
{
    deleteId = m_events[index].id;

    QVariantMap parameters;
    parameters["calendarId"] = m_calendars[currentCalendar].id.c_str();
    parameters["eventId"] = m_events[index].id.c_str();

    Google::query(
        "https://www.googleapis.com/calendar/v3/calendars/calendarId/events/eventId",
        parameters, "DELETE", {}, QueryType::DeleteEvent
    );
}

void CalendarPresenter::clearClipboard()
{
    clipboard_event = CalendarEvent{};

    view->setEventList(m_events, clipboard_event);
}

void CalendarPresenter::durationChange(int eventIdx, int duration)
{
    m_events[eventIdx].end = m_events[eventIdx].start.addSecs(duration * 60);

    sendEventQuery(m_events[eventIdx]);
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

void CalendarPresenter::requestEvents()
{
    auto& calendar = m_calendars[currentCalendar];

    QVariantMap param;
    param["calendarId"] = QString(calendar.id.c_str());
    param["timeMin"] = shownWeek.first.toString("yyyy-MM-dd") + "T00:00:00Z";
    param["timeMax"] = shownWeek.second.toString("yyyy-MM-dd") + "T23:59:59Z";

    Google::query(
        "https://www.googleapis.com/calendar/v3/calendars/calendarId/events",
        param, "GET", {}, QueryType::GetEvents
    );
}

void CalendarPresenter::setClipboard(const CalendarEvent& e)
{
    clipboard_event = e;

    std::sort(m_events.begin(), m_events.end(),
        [](const CalendarEvent& left, const CalendarEvent& right)
        {
            return left.start < right.start;
        }
    );

    view->setEventList(m_events, clipboard_event);
}

void CalendarPresenter::sendEventQuery(const CalendarEvent& event)
{
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
