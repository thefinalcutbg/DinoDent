#include "PrintPreviewDialog.h"
#include <QGraphicsItem>
#include <QScrollBar>
#include <QScreen>
#include "View/CommonIcon.h"
#include "View/ModalDialogBuilder.h"   

PrintPreviewDialog::PrintPreviewDialog(LimeReport::ReportEngine& r)
	: QDialog(nullptr)
	, ui(new Ui::PrintPreviewDialogClass())
{
	ui->setupUi(this);

	setWindowTitle("Предварителен преглед");

    setWindowFlag(Qt::WindowMaximizeButtonHint, true);

	int dialogWidth = 1100;
	int dialogHeight = 850;

    QRect available = screen()->availableGeometry();

    if(dialogHeight > available.height())
    {
        dialogHeight = available.height() - 20;
	}

    if (dialogWidth > available.width())
    {
        dialogWidth = available.width() - 10;
    }

    resize(QSize(dialogWidth, dialogHeight));
 
    setWindowFlag(Qt::WindowMaximizeButtonHint, true);

	ui->printButton->setIcon(CommonIcon::getPixmap(CommonIcon::Type::PRINT));

    ui->view->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    ui->view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    ui->view->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    scene = r.createPreviewScene(ui->view);

    if (!scene) return;

    ui->view->setScene(scene);

    QRectF pagesBounds;

    for (QGraphicsItem* item : scene->items()) {
        if (item->parentItem()) continue;

        QRectF rect = item->sceneBoundingRect();
        if (rect.isEmpty()) continue;

        pageRects.append(rect);
        pagesBounds = pagesBounds.isNull() ? rect : pagesBounds.united(rect);
    }

    std::sort(pageRects.begin(), pageRects.end(), [](const QRectF& a, const QRectF& b) { return a.top() < b.top(); });

    scene->setSceneRect(pagesBounds);

    pageCount = pageRects.size();
    ui->pageSpin->setMaximum(pageCount);
    ui->pageSpin->setSuffix(QString("/ %1").arg(pageCount));

    QObject::connect(ui->pageSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [&](int page) { gotoPage(page - 1); });
    QObject::connect(ui->view->verticalScrollBar(), &QScrollBar::valueChanged, this, [&](int) { detectCurrentPage(); });

    QObject::connect(ui->printButton, &QPushButton::clicked, this, [&]() { accept(); });

    class ResizeFilter : public QObject
    {
    public:
        using QObject::QObject;
        std::function<void()> callback;

    protected:
        bool eventFilter(QObject* object, QEvent* event) override {
            if (event->type() == QEvent::Resize && callback) callback();
            return QObject::eventFilter(object, event);
        }
    };

    auto* resizeFilter = new ResizeFilter(ui->view);
    resizeFilter->callback = [&]() { fitSceneWidth(); };
    ui->view->viewport()->installEventFilter(resizeFilter);

    updatePageControls();
    QTimer::singleShot(0, this, [&]() { fitSceneWidth(); gotoPage(0); });

}

void PrintPreviewDialog::fitSceneWidth()
{
    if (pageRects.isEmpty()) return;

    QRectF bounds = pageRects.first();
    for (int i = 1; i < pageRects.size(); ++i) bounds = bounds.united(pageRects[i]);

    if (bounds.width() <= 0) return;

    constexpr qreal margin = 30.0;
    qreal availableWidth = ui->view->viewport()->width() - margin * 2.0;
    if (availableWidth <= 0) return;

    qreal currentY = ui->view->mapToScene(QPoint(0, ui->view->viewport()->height() / 2)).y();

    ui->view->resetTransform();

    qreal factor = availableWidth / bounds.width();
    ui->view->scale(factor, factor);

    qreal visibleWidth = ui->view->viewport()->width() / factor;

    ui->view->setSceneRect(
        bounds.center().x() - visibleWidth / 2.0,
        bounds.top(),
        visibleWidth,
        bounds.height()
        );

    ui->view->centerOn(bounds.center().x(), currentY);
}

void PrintPreviewDialog::gotoPage(int page)
{
     page = qBound(0, page, pageCount - 1);
     currentPage = page;

    const QRectF& rect = pageRects[currentPage];
    QPoint p = ui->view->mapFromScene(QPointF(rect.center().x(), rect.top()));

    ui->view->verticalScrollBar()->setValue(ui->view->verticalScrollBar()->value() + p.y() - 10);
    updatePageControls();
}

void PrintPreviewDialog::updatePageControls()
{
    QSignalBlocker blocker(ui->pageSpin);
    ui->pageSpin->setValue(currentPage + 1);
}

void PrintPreviewDialog::detectCurrentPage()
{
    QPointF position = ui->view->mapToScene(QPoint(ui->view->viewport()->width() / 2, ui->view->viewport()->height() / 3));

    int closestPage = 0;
    qreal closestDistance = std::numeric_limits<qreal>::max();

    for (int i = 0; i < pageRects.size(); ++i) {
        const QRectF& rect = pageRects[i];
        if (rect.contains(position)) { closestPage = i; break; }

        qreal distance = std::abs(rect.center().y() - position.y());
        if (distance < closestDistance) { closestDistance = distance; closestPage = i; }
    }

    if (closestPage != currentPage) { currentPage = closestPage; updatePageControls(); }
}

PrintPreviewDialog::~PrintPreviewDialog()
{
	delete ui;
}

