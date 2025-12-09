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
    QString qApptStr = QString::fromStdString(appointmentDateTime);
    
    QDateTime apptDt = QDateTime::fromString(qApptStr, SMSMessage::expectedFormat);

    if (!apptDt.isValid()) {        
        this->phone = "";
        this->message = "";
        return;
    }

    if (hoursBefore) {
        this->toDate = apptDt.addSecs(-hoursBefore * 60 * 60).toString("yyyy-MM-dd HH:mm").toStdString();
    }

    static const QString weekdays[7] = {
        "Понеделник",
        "Вторник",
        "Сряда",
        "Четвъртък",
        "Петък",
        "Събота",
        "Неделя"
	};

    const QString dateStr = apptDt.date().toString("dd.MM.yyyy");
    const QString timeStr = apptDt.time().toString("HH:mm");
    const QString doctorName = "д-р " + QString(User::doctor().lname.c_str());
    const QString doctorPhone = User::doctor().phone.c_str();
    const QString weekdayStr = weekdays[apptDt.date().dayOfWeek()];

    qMsgTmpl.replace("<ДАТА>", dateStr, Qt::CaseSensitive);
    qMsgTmpl.replace("<ЧАС>", timeStr, Qt::CaseSensitive);
    qMsgTmpl.replace("<ЛДМ>", doctorName, Qt::CaseSensitive);
    qMsgTmpl.replace("<ТЕЛ>", doctorPhone, Qt::CaseSensitive);
	qMsgTmpl.replace("<ДЕН>", weekdayStr, Qt::CaseSensitive);

    this->message = qMsgTmpl.toStdString();
}