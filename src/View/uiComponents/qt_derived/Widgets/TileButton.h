#pragma once

#include <QAbstractButton>
#include <QWidget>
#include <QLabel>
#include <QPainter>
#include <QGridLayout>
#include <QMouseEvent>
#include <QDebug>

#include "Model/Patient.h"

class TileButton : public QAbstractButton
{


    void paintEvent(QPaintEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* e) override;
    

protected:

    bool hover;
    bool clicked;
    bool m_reveresed{ false };
    QFont header;
    QFont info;
    QFont infoLabel;

    QString elide(const QString& text, int length);
    virtual void paintInfo(QPainter* painter) = 0;

public:
    TileButton(QWidget* parent = 0);
    void reverse() { m_reveresed = !m_reveresed; };
};

class PatientTile : public TileButton
{
    QString idLabel;
    QString name;
    QString type;
    QString id;
    QString hirbNo;
    QString birthDate;
    QString age;
    QString address;
    QString phone;

    void paintInfo(QPainter* painter);

public:
    PatientTile(QWidget* parent = 0);
    void setData(const Patient& patient, Date currentDate);

};

class AllergiesTile : public TileButton
{
    
    QString allergiesLabel;
    QString currentDLabel;
    QString pastDLabel;
    QString noInfo;
    
    QString allergies;
    QString currentDiseases;
    QString pastDiseases;


    void paintInfo(QPainter* painter);

public:
    AllergiesTile(QWidget* parent = 0);
    void setData(const Patient& patient);

};