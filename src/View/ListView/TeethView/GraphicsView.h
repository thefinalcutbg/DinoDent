#pragma once

#include <QGraphicsView>
#include <QMouseEvent>
#include <QOpenGLWidget>
class GraphicsView : public QGraphicsView
{
	Q_OBJECT

	

	void mousePressEvent(QMouseEvent* event) override;
	//void scrollContentsBy(int, int) override {  };

	void resizeEvent(QResizeEvent*) {
		fitView();
	}

	void showEvent(QShowEvent*) {
		fitView();
	}

	void fitView() {
		const QRectF rect = QRectF(-20, -2, 721, 451);
		fitInView(rect,
			Qt::KeepAspectRatio);
		setSceneRect(rect);
	}

public:
	GraphicsView(QWidget *parent);
	QOpenGLWidget* gl;
	~GraphicsView();
};
