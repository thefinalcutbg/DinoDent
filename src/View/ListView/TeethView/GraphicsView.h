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


	void focusInEvent(QFocusEvent* event) override { 

		setStyleSheet(
			"border-style: solid;"
			"border-color: rgb(102, 174, 229);"
			"border-width: 1px;"
			"border-radius: 4px;"
		);
	}

	void focusOutEvent(QFocusEvent* event) override { 

		setStyleSheet(
			"border-style: solid;"
			"border-color: lightgray;"
			"border-width: 1px;"
			"border-radius: 4px;"
		);
	}

	void wheelEvent(QWheelEvent* event) override;

public:
	GraphicsView(QWidget *parent);

	~GraphicsView();

signals:
	void focusLost();
};
