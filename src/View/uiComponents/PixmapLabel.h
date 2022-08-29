#pragma once

#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

class PixmapLabel : public QLabel
{
    Q_OBJECT
public:
    explicit PixmapLabel(QWidget* parent = 0);
    virtual int heightForWidth(int width) const;
    virtual QSize sizeHint() const;
    QPixmap scaledPixmap() const;
public slots:
    void setPixmap(const QPixmap&);
    void resizeEvent(QResizeEvent*);
private:
    QPixmap pix;
};

