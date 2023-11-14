#include "SurfacePanel.h"

#include "Presenter/SurfacePanelPresenter.h"
#include "View/Graphics/ToothPainter.h"


SurfacePanel::SurfacePanel(QWidget* parent)
	: QWidget(parent), presenter(NULL)
{
	ui.setupUi(this);

	//QOpenGLWidget* gl = new QOpenGLWidget();
//	QSurfaceFormat format;

//	format.setSamples(16);
//	gl->setFormat(format);
//	ui.surfaceView->setViewport(gl);

	occlusal_scene = new QGraphicsScene;
	ui.occlusalView->setScene(occlusal_scene);
	occlusal_scene->setBackgroundBrush(Qt::white);
	ui.occlusalView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	occlusal_toothGraphic = new CPTooth;

	occlusal_scene->addItem(occlusal_toothGraphic);

	for (int i = 0; i < 5; i++) {
		occlusal_polygon[i] = new ControlPanelPolygon(static_cast<ButtonPos>(i), this);
		occlusal_scene->addItem(occlusal_polygon[i]);
	}


	buccal_scene = new QGraphicsScene;
	ui.buccalView->setScene(buccal_scene);
	buccal_scene->setBackgroundBrush(Qt::white);
	ui.buccalView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	buccal_toothGraphic = new CPTooth;

	buccal_scene->addItem(buccal_toothGraphic);

	for (int i = 0; i < 5; i++) {
		buccal_polygon[i] = new ControlPanelPolygon(static_cast<ButtonPos>(i+5), this);
		buccal_scene->addItem(buccal_polygon[i]);
	}


}

SurfacePanel::~SurfacePanel()
{
}

void SurfacePanel::setPresenter(SurfacePanelPresenter* presenter)
{
	this->presenter = presenter;
	presenter->setView(this);
}

void SurfacePanel::paintTooth(const ToothPaintHint& tooth)
{
	occlusal_toothGraphic->setPixmap(ToothPainter::getSurfacePanel(tooth));
	buccal_toothGraphic->setPixmap(ToothPainter::getSurfacePanel(tooth, false));
}

void SurfacePanel::hidePanel(bool hidden)
{
	setHidden(hidden);
}

void SurfacePanel::setLabels(std::array<std::string, 10> surfaceNames)
{
	for (int i = 0; i < 10; i++) {
		labels[i] = QString::fromStdString(surfaceNames[i]);
	}

	//ui.sideBox->setText(labels[5]+":");
}

void SurfacePanel::setSideButtonsClicked(bool obturation, bool caries)
{
	//ui.sideObturation->setChecked(obturation);
	//ui.sideCaries->setChecked(caries);
}

void SurfacePanel::buttonHovered(ButtonPos position, Hover hoverState)
{
	if (hoverState == Hover::out)
	{
		ui.statusLabel->setText("");
	}
	else
	{
		ui.statusLabel->setText(labels[static_cast<int>(position)]);
	}

}

void SurfacePanel::buttonClicked(ButtonPos position, MouseClick click)
{
	if (click == MouseClick::rightClick)
		presenter->buttonClicked(position, SurfaceClick::rightClick);
	else if (click == MouseClick::leftClick)
		presenter->buttonClicked(position, SurfaceClick::leftClick);

	ui.statusLabel->setText(labels[static_cast<int>(position)]);
}
