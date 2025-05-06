#pragma once

#include <QAbstractButton>
#include <QWidget>
#include <QLabel>
#include <QPainter>
#include <QGridLayout>
#include <QMouseEvent>

#include "Model/Patient.h"

class TileButton : public QAbstractButton
{
    void paintEvent(QPaintEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* e) override;
    
protected:

    bool hover{ false };
    bool clicked{ false };
    bool m_reveresed{ false };
    QFont header;
    QFont info;
    QFont infoLabel;

    QString elide(const QString& text, int length);
    virtual void paintInfo(QPainter* painter) = 0;

public:
    TileButton(QWidget* parent = 0);
    void reverse() { m_reveresed = !m_reveresed; update(); };

};

#include "View/uiComponents/IconButton.h"

struct PatientTile : public TileButton
{

public:
    IconButton* nraButton;
    IconButton* notesButton;
    IconButton* appointmentButton;
    IconButton* notificationButton;
    PatientTile(QWidget* parent = 0);
    void setData(const Patient& patient, int age);

private:

    QString idLabel;
    QString name;
    QString type;
    QString id;
    QString hirbNo;
    QString birthDate;
    QString age;
    QString address;
    QString phone;
    QPixmap* zodiac{ nullptr };
    bool birthday{ false };

    static constexpr int iconSize = 32;

    void paintInfo(QPainter* painter);

    void resizeEvent(QResizeEvent* event);

};

class MedStatusTile : public TileButton
{

    static inline const QString 
        allergiesLabel{ "Алергии:" }, 
        currentDLabel{ "Настоящи заболявания:" },
        pastDLabel{ "Минали заболявания:" };
    
    QString allergies; 
    QString currentDiseases;
    QString pastDiseases;

    static constexpr int buttonSize = 30;

    void paintInfo(QPainter* painter) override;
    void resizeEvent(QResizeEvent* event) override;
public:
    IconButton* nhifButton;
    IconButton* hisButton;
    IconButton* hospitalizedButton;
    MedStatusTile(QWidget* parent = 0);
    void setData(const Patient& patient);

};
