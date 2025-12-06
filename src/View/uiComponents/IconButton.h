#pragma once

#include <QPushButton>
#include <QColor>

class QVariantAnimation;

class IconButton : public QPushButton
{
    Q_OBJECT

    bool m_hover{ false };
    bool m_clicked{ false };

    QColor hoverColor;
    QColor backgroundColor;
    QColor outlineColor;
    bool m_monochrome{ false };

    qreal m_hoverProgress{ 0.0 };
    QVariantAnimation* m_hoverAnimation{ nullptr };

    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

public:
    explicit IconButton(QWidget* parent = nullptr);

    // sometimes it needs to be changed depending on the background of the parent widget
    void setHoverColor(const QColor& color);
    void setBackgroundColor(const QColor& color);
    void setMonochrome(bool monochrome);

    void setOutlineColor(const QColor& color) {
        outlineColor = color;
        update();
    }

    ~IconButton() override;
};
