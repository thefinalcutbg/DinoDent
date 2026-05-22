#pragma once

#include <QPushButton>
#include <QColor>

class QVariantAnimation;
class QLabel;

class BigPushButton : public QPushButton
{
    Q_OBJECT

    bool m_hover{ false };
    bool m_clicked{ false };
    QColor hoverColor;
    QColor normalColor;

    // Hover color animation (0 = normalColor, 1 = hoverColor)
    qreal hoverProgress{ 0.0 };
    QVariantAnimation* hoverAnimation{ nullptr };

    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* e) override;
    QSize minimumSizeHint() const override;

	QLabel* richTextLabel{ nullptr };

public:
    BigPushButton(QWidget* parent);
    void setHoverColor(const QColor& color);
    void setNormalColor(const QColor& color);
	void setRichText(const QString& text);
	void setNormalText(const QString& text);
    ~BigPushButton();
};
