#include "GraphicsView.h"
#include <QApplication>
#include <View/Theme.h>
#include <QPainter>
#include <QGraphicsItem>
GraphicsView::GraphicsView(QWidget *parent)
	: QGraphicsView(parent)
{
	
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	setCacheMode(QGraphicsView::CacheBackground);
	setDragMode(QGraphicsView::RubberBandDrag);

    setStyleSheet("QGraphicsView { background: transparent;}");
	
	setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::FullViewportUpdate);
	
}

void GraphicsView::disableMultiSelection()
{
	setDragMode(QGraphicsView::DragMode::NoDrag);
}

void GraphicsView::mousePressEvent(QMouseEvent* event)
{
	auto previouslySelectedList = scene()->selectedItems();

	QGraphicsView::mousePressEvent(event);
	
	if (event->button() == Qt::LeftButton &&
		!QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {

		QGraphicsView::mousePressEvent(event);

		auto newSelectedList = scene()->selectedItems();

		if (newSelectedList.empty()) return;

		//Аllows double click on multiple selected teeth:
		if (previouslySelectedList.contains(newSelectedList[0])) {
			for (auto i : previouslySelectedList) {
				i->setSelected(true);
			}
		}
		
	}
	
}

GraphicsView::~GraphicsView()
{
}

void GraphicsView::wheelEvent(QWheelEvent* event)
{
	event->ignore();
}
