#pragma once

#include <QAbstractButton>
#include <QWidget>
#include <QLabel>
#include <QPainter>
#include <QGridLayout>
#include <QMouseEvent>

#include "Model/Patient.h"

class QVariantAnimation;

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

    qreal m_hoverProgress{ 0.0 };
    QVariantAnimation* m_hoverAnimation{ nullptr };

    QString elide(const QString& text, int length);
    virtual void paintInfo(QPainter* painter) = 0;

    QColor animatedColor(const QColor& normal, const QColor& hover) const;

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
    IconButton* menuButton;
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
    MedStatusTile(QWidget* parent = 0);
    void setData(const Patient& patient);

};
