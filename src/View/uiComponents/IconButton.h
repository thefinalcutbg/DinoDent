#pragma once

#include <QPushButton>
#include <QColor>

class QVariantAnimation;

class IconButton : public QPushButton
{
    Q_OBJECT

public:
    enum IconMode { MONOCHROME, COLOR, COLOR_ON_HOVER_ONLY };

private:

    QString m_badgeText;
    bool m_hover{ false };
    bool m_clicked{ false };
    bool m_shring_on_click{ true };

    QColor hoverColor;
    QColor backgroundColor;
    QColor outlineColor;
	bool m_colorOnHoverOnly{ false };

    qreal m_hoverProgress{ 0.0 };
    QVariantAnimation* m_hoverAnimation{ nullptr };

    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

public:

    explicit IconButton(QWidget* parent = nullptr);

    void setHoverColor(const QColor& color);
    void setBackgroundColor(const QColor& color);

    void setIconMode(IconMode mode) { 
        m_iconMode = mode;
        update();

	}
    void setOutlineColor(const QColor& color) {
        outlineColor = color;
        update();
    }

    void setAlert(std::string text) {
		m_badgeText = QString::fromStdString(text);
        update();
    }

    void setShrinkOnClick(bool shrink) {
        m_shring_on_click = shrink;
        update();
    }

    ~IconButton() override;

private:
	IconMode m_iconMode{ COLOR };
};
