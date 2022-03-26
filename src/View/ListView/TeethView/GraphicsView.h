#pragma once

#include <QGraphicsView>
#include <QMouseEvent>

class GraphicsView : public QGraphicsView
{
	Q_OBJECT

	void mousePressEvent(QMouseEvent* event) override;
	
	void wheelEvent(QWheelEvent* event) override;

public:
	GraphicsView(QWidget *parent);

	~GraphicsView();

signals:
	void focusLost();
};
