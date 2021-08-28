#pragma once

#include <QGraphicsView>
#include <QMouseEvent>
#include <QOpenGLWidget>
#
class GraphicsView : public QGraphicsView
{
	Q_OBJECT

	QOpenGLWidget* gl;

	void mousePressEvent(QMouseEvent* event) override;


	void focusInEvent(QFocusEvent* event) override { setStyleSheet("border: 1px solid lightblue"); }

	void focusOutEvent(QFocusEvent* event) override { setStyleSheet("");}

	void wheelEvent(QWheelEvent* event) override;

public:
	GraphicsView(QWidget *parent);

	~GraphicsView();

signals:
	void focusLost();
};
