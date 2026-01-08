#include "TileButton.h"
#include <QPainterPath>
#include <QMouseEvent>
#include <QApplication>
#include <QVariantAnimation>
#include <QEasingCurve>

#include "View/Theme.h"
#include "View/Graphics/Zodiac.h"
#include "Model/FreeFunctions.h"
#include "Model/User.h"

TileButton::TileButton(QWidget* parent) : QAbstractButton(parent)
{
    header.setBold(true);
    header.setPointSizeF(info.pointSizeF() + 6);
    infoLabel.setBold(true);

    installEventFilter(this);

    setCursor(QCursor(Qt::PointingHandCursor));

    m_hoverAnimation = new QVariantAnimation(this);
    m_hoverAnimation->setDuration(150);
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_hoverAnimation->setStartValue(0.0);
    m_hoverAnimation->setEndValue(0.0);

    connect(m_hoverAnimation, &QVariantAnimation::valueChanged, this,
        [this](const QVariant& value) {
            m_hoverProgress = value.toReal();
            update();
        });
}

QColor TileButton::animatedColor(const QColor& normal, const QColor& hover) const
{
    auto mixChannel = [](int a, int b, qreal t) -> int {
        return a + qRound((b - a) * t);
        };

    qreal t = m_hoverProgress;
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;

    if (clicked)
        t = 0.0;

    QColor result;
    result.setRed(mixChannel(normal.red(), hover.red(), t));
    result.setGreen(mixChannel(normal.green(), hover.green(), t));
    result.setBlue(mixChannel(normal.blue(), hover.blue(), t));
    result.setAlpha(mixChannel(normal.alpha(), hover.alpha(), t));
    return result;
}

void TileButton::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
/*
    auto path = Theme::getHalfCurvedPath(width(), height());

    if (m_reveresed) {
        QTransform mirror(-1, 0, 0, 0, 1, 0, 0, 0, 1);
        painter.setTransform(mirror);
        painter.translate(-width(), 0);
    }

    painter.fillPath(path, Theme::sectionBackground);

    QPen pen(Theme::border);
    pen.setCosmetic(true);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawPath(path);

    if (m_reveresed) {
        painter.resetTransform();
    }
*/
    QColor textColor = animatedColor(Theme::fontTurquoise, Theme::fontTurquoiseClicked);
    painter.setPen(QPen(textColor));

    paintInfo(&painter);

    painter.end();
}

bool TileButton::eventFilter(QObject*, QEvent* e)
{
    if (e->type() == QEvent::Enter) {
        hover = true;

        if (m_hoverAnimation) {
            m_hoverAnimation->stop();
            m_hoverAnimation->setStartValue(m_hoverProgress);
            m_hoverAnimation->setEndValue(1.0);
            m_hoverAnimation->start();
        }
        else {
            m_hoverProgress = 1.0;
            update();
        }
    }

    if (e->type() == QEvent::Leave) {

        hover = false;

        if (m_hoverAnimation) {
            m_hoverAnimation->stop();
            m_hoverAnimation->setStartValue(m_hoverProgress);
            m_hoverAnimation->setEndValue(0.0);
            m_hoverAnimation->start();
        }
        else {
            m_hoverProgress = 0.0;
            update();
        }
    }

    if (e->type() == QEvent::MouseButtonPress) {
        auto me = static_cast<QMouseEvent*>(e);
        if (me->button() == Qt::LeftButton) {
            clicked = true;
            update();
        }
    }

    if (e->type() == QEvent::MouseButtonRelease) {
        auto me = static_cast<QMouseEvent*>(e);

        if (me->button() == Qt::RightButton) {
            emit customContextMenuRequested(mapToGlobal(me->pos()));
        }
        else {
            clicked = false;
            update();
        }
    }

    return false;
}

QString TileButton::elide(const QString& text, int length)
{
    if (text.length() < length)
        return text;

    return text.chopped(text.length() - length) + "...";
}


PatientTile::PatientTile(QWidget* parent) : TileButton(parent)
{
    nraButton = new IconButton(this);
    nraButton->setIcon(QIcon(":/icons/icon_nra.png"));
    nraButton->move(5, 5);
    nraButton->setFixedSize(iconSize, iconSize);

    appointmentButton = new IconButton(this);
    appointmentButton->setIcon(QIcon(":/icons/icon_calendar.png"));
    appointmentButton->setFixedSize(iconSize, iconSize);
    appointmentButton->move(width() - (iconSize + 5), 5);
    appointmentButton->setToolTip("Запази посещение");

    notificationButton = new IconButton(this);
    notificationButton->setIcon(QIcon(":/icons/icon_bell.png"));
    notificationButton->setFixedSize(iconSize, iconSize);
    notificationButton->move(appointmentButton->x() - 40, appointmentButton->y());
    notificationButton->setToolTip("Добавяне на напомняне");

    notesButton = new IconButton(this);
    notesButton->setIcon(QIcon(":/icons/icon_notes.png"));
    notesButton->setFixedSize(iconSize, iconSize);
    notesButton->move(notificationButton->x() - 40, notificationButton->y());
    notesButton->setToolTip("Бележки за пациента");
}


void PatientTile::paintInfo(QPainter* painter)
{
    constexpr int rowYPos[3]{ 60,80,100 };

    painter->setFont(infoLabel);
    painter->drawText(20, rowYPos[0], idLabel);
    painter->drawText(20, rowYPos[1], "Номер на ЗОК: ");
    painter->drawText(20, rowYPos[2], "Рождена дата: ");

    painter->drawText(width() / 2, rowYPos[0], "Телефон: ");
    painter->drawText(width() / 2, rowYPos[1], "Адрес: ");
    painter->drawText(width() / 2, rowYPos[2], "Възраст: ");

    QFontMetrics metric(infoLabel);

    auto horizontalAdvance = [metric](const QString& label) {
        return metric.horizontalAdvance(label);
        };

    painter->setFont(info);
    painter->drawText(20 + horizontalAdvance(idLabel), rowYPos[0], id);
    painter->drawText(20 + horizontalAdvance("Номер на ЗОК: "), rowYPos[1], hirbNo);
    painter->drawText(20 + horizontalAdvance("Рождена дата: "), rowYPos[2], birthDate);

    painter->drawText(width() / 2 + horizontalAdvance("Телефон: "), rowYPos[0], phone);
    painter->drawText(width() / 2 + horizontalAdvance("Адрес: "), rowYPos[1], address);
    painter->drawText(width() / 2 + horizontalAdvance("Възраст: "), rowYPos[2], age);

    painter->setFont(header);
    painter->setPen(QPen(animatedColor(Theme::fontRed, Theme::fontRedClicked)));

    int nameXPos = 20;

    if (!nraButton->isHidden()) {
        nameXPos = nraButton->x() + iconSize + 5;
    }

    painter->drawText(nameXPos, 27, name);

    painter->setRenderHint(QPainter::Antialiasing);

    if (zodiac) painter->drawPixmap(width() - 37, height() - 36, 32, 32, *zodiac);

    static QPixmap bdayPx{ ":/icons/icon_bday.png" };

    if (birthday) painter->drawPixmap(20 + horizontalAdvance("Рождена дата: 00.00.0000 г"), 86, 15, 15, bdayPx);

}

void PatientTile::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    appointmentButton->move(width() - 5 - appointmentButton->width(), 5);
    notificationButton->move(appointmentButton->x() - 40, appointmentButton->y());
    notesButton->move(notificationButton->x() - 40, notificationButton->y());

    update();
}

void PatientTile::setData(const Patient& patient, int age)
{

    nraButton->setHidden(!User::hasNhifContract());

    name = patient.fullName().c_str();
    if (name.size() > 32) {
        name = patient.firstLastName().c_str();
    }

    if (name.size() > 32) {
        name = elide(QString::fromStdString(patient.fullName()), 30);
    }

    switch (patient.type)
    {
    case Patient::EGN: idLabel = "ЕГН: "; break;
    case Patient::LNCH: idLabel = "ЛНЧ: "; break;
    case Patient::SSN: idLabel = "ССН: "; break;
    case Patient::EU: idLabel = "ID: "; break;
    default: idLabel = "???: "; break;
    }

    id = QString::fromStdString(patient.id);

    if (patient.HIRBNo == "")
        hirbNo = "Няма данни";
    else hirbNo = QString::fromStdString(patient.HIRBNo);

    birthDate = QString::fromStdString((patient.birth.toBgStandard(true)));

    this->age = QString::number(age) + " г.";

    address = elide(QString::fromStdString(patient.getFullAddress()), 28);

    if (patient.phone != "")
        phone = QString::fromStdString(patient.phone);
    else phone = "";

    if (patient.insuranceStatus)
    {
        switch (patient.insuranceStatus->status)
        {
        case Insured::Yes:
            nraButton->setBackgroundColor(QColor(101, 199, 208));
            nraButton->setHoverColor(QColor(80, 152, 154));
            nraButton->setToolTip("Пациентът е здравноосигурен");
            break;
        case Insured::No:
            nraButton->setBackgroundColor(Theme::fontRed);
            nraButton->setHoverColor(Theme::fontRedClicked);
            nraButton->setToolTip("Пациентът няма здравни осигуровки");
            break;
        case Insured::NoData:
            nraButton->setBackgroundColor(QColor(255, 165, 0));
            nraButton->setHoverColor(QColor(255, 165, 0));
            nraButton->setToolTip("За този пациент няма данни в НАП");
            break;
        }
    }
    else
    {
        nraButton->setBackgroundColor(Theme::sectionBackground);
        nraButton->setHoverColor(Theme::background);
        nraButton->setToolTip("Проверка на здравноосигурителен статус");

    }

    notesButton->setMonochrome(patient.patientNotes.empty());

    birthday = patient.birth.isSameDayInTheYear();

    zodiac = Zodiac::getPixmap(patient.birth.day, patient.birth.month);

    update();
}


MedStatusTile::MedStatusTile(QWidget* parent) :
    TileButton(parent)
{
    reverse();
    header.setPointSizeF(10);
}

void MedStatusTile::setData(const Patient& patient)
{

    auto listToString = [](const std::vector<MedicalStatus>& list, const std::string& emptyList)
        {
            std::string result;

            for (auto& str : list) {
                result += str.diagnosis.name();
                result += ", ";
            }

            if (result.size()) {
                result.pop_back();
                result.pop_back();
            }
            else {
                result = emptyList;
            };

            return result;
        };

    allergies = elide(patient.getAllergiesStr().c_str(), 30);
    currentDiseases = elide(listToString(patient.medStats.condition, "Не съобщава").c_str(), 30);
    pastDiseases = elide(listToString(patient.medStats.history, "Не съобщава").c_str(), 30);

    update();
}

void MedStatusTile::paintInfo(QPainter* painter)
{
    painter->setFont(header);
    painter->drawText(20, 20, allergiesLabel);
    painter->drawText(20, 58, currentDLabel);
    painter->drawText(20, 96, pastDLabel);

    painter->setFont(info);
    painter->drawText(20, 37, allergies);
    painter->drawText(20, 75, currentDiseases);
    painter->drawText(20, 113, pastDiseases);

}

void MedStatusTile::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    update();
}
