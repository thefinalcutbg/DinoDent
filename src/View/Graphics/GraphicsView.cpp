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
    if (dragMode() != QGraphicsView::RubberBandDrag) {
        QGraphicsView::mousePressEvent(event);
        return;
    }

    QGraphicsItem* clickedItem = itemAt(event->pos());

    const auto previouslySelectedList = scene()->selectedItems();

    QGraphicsView::mousePressEvent(event);

    if (event->button() == Qt::LeftButton &&
        !(event->modifiers() & Qt::ControlModifier)) 
    {
    
        if (scene()->selectedItems().isEmpty() && clickedItem) {
            clickedItem->setSelected(true);
        }

        auto newSelectedList = scene()->selectedItems();
        if (newSelectedList.isEmpty()) return;

        if (previouslySelectedList.contains(newSelectedList.first())) {
            for (auto* i : previouslySelectedList) {
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
