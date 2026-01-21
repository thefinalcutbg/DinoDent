#include "SMSMessage.h"
#include "Model/User.h"

#include <QString>
#include <QDateTime>

SMSMessage::SMSMessage(
    const std::string& phone,
    const std::string& msgTemplate,
    const std::string& appointmentDateTime,
    int hoursBefore
) : 
    phone(phone)
{
    QString qMsgTmpl = QString::fromStdString(msgTemplate);



    const QString doctorName = "д-р " + QString(User::doctor().lname.c_str());
    const QString doctorPhone = User::doctor().phone.c_str();

    qMsgTmpl.replace("<ЛДМ>", doctorName, Qt::CaseSensitive);
    qMsgTmpl.replace("<ТЕЛ>", doctorPhone, Qt::CaseSensitive);

    QDateTime apptDt = QDateTime::fromString(appointmentDateTime.c_str(), SMSMessage::expectedFormat);

    if (!apptDt.isValid()) {
        this->message = qMsgTmpl.toStdString();
        return;
    }

    static const QString weekdays[7] = {
    "понеделник",
    "вторник",
    "сряда",
    "четвъртък",
    "петък",
    "събота",
    "неделя"
    };

    if (hoursBefore) {
        this->toDate = apptDt.addSecs(-hoursBefore * 60 * 60).toString("yyyy-MM-dd HH:mm").toStdString();
    }

    const QString dateStr = apptDt.date().toString("dd.MM.yyyy");
    const QString timeStr = apptDt.time().toString("HH:mm");
    const QString weekdayStr = weekdays[apptDt.date().dayOfWeek() - 1];

    qMsgTmpl.replace("<ДАТА>", dateStr, Qt::CaseSensitive);
    qMsgTmpl.replace("<ЧАС>", timeStr, Qt::CaseSensitive);
    qMsgTmpl.replace("<ДЕН>", weekdayStr, Qt::CaseSensitive);
    
    this->message = qMsgTmpl.toStdString();
}