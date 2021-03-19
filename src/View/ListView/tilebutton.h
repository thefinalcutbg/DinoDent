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
    bool hover;
    bool clicked;

    void paintEvent(QPaintEvent* e) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    

protected:
    Patient* patient;
    QFont header;
    QFont info;

    QString elide(const QString& text, int length);
    virtual void paintInfo(QPainter* painter) = 0;

public:
    TileButton(QWidget* parent = 0);
    void setPatient(Patient &patient);
    virtual void refresh() = 0;
};

class PatientTile : public TileButton
{
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
    void refresh();

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
    void refresh();

};
