#include "SurfacePanel.h"

#include <QOpenGLWidget>

#include "Presenter/ListPresenter/ToothHintCreator.h"
#include "Presenter/ListPresenter/SurfacePanel/SurfacePanelPresenter.h"
#include "../../ToothPaintDevices/ToothPainter.h"


SurfacePanel::SurfacePanel(QWidget* parent)
	: QWidget(parent), presenter(NULL)
{
	ui.setupUi(this);

	//QOpenGLWidget* gl = new QOpenGLWidget();
//	QSurfaceFormat format;

//	format.setSamples(16);
//	gl->setFormat(format);
//	ui.surfaceView->setViewport(gl);

	scene = new QGraphicsScene;
	ui.surfaceView->setScene(scene);
	scene->setBackgroundBrush(Qt::white);
	ui.surfaceView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	toothGraphic = new CPTooth;

	ui.sideBox->setStyleSheet("color: rgb(2, 127, 128); font-weight: bold;");

	ui.sideCaries->pathology = true;
	
	scene->addItem(toothGraphic);

	for (int i = 0; i < 5; i++) {
		polygon[i] = new ControlPanelPolygon(static_cast<ButtonPos>(i), this);
		scene->addItem(polygon[i]);
	}

	connect(ui.sideObturation, &QPushButton::clicked, [=] { presenter->sideObturationClicked();  });
	connect(ui.sideCaries, &QPushButton::clicked, [=] { presenter->sideCariesClicked(); });
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
	
	toothGraphic->setPixmap(ToothPainter::getOcclusal(tooth));
}

void SurfacePanel::hidePanel(bool hidden)
{
	setHidden(hidden);
}

void SurfacePanel::setLabels(std::array<std::string, 6> surfaceNames)
{
	for (int i = 0; i < 6; i++) {
		labels[i] = QString::fromStdString(surfaceNames[i]);
	}

	ui.sideBox->setText(labels[5]+":");
}

void SurfacePanel::setStatuses(std::array<std::string, 6> StatusNames)
{
	for (int i = 0; i < 6; i++) {
		statuses[i] = QString::fromStdString(StatusNames[i]);
	}
}

void SurfacePanel::setSideButtonsClicked(bool obturation, bool caries)
{
	ui.sideObturation->setChecked(obturation);
	ui.sideCaries->setChecked(caries);
}

void SurfacePanel::buttonHovered(ButtonPos position, Hover hoverState)
{
	if (hoverState == Hover::out)
	{
		ui.surfaceInfoLabel->setText("");
		ui.statusInfoLabel->setText("");
	}
	else
	{
		ui.surfaceInfoLabel->setText(labels[static_cast<int>(position)]);
		ui.statusInfoLabel->setText(statuses[static_cast<int>(position)]);
	}

}

void SurfacePanel::buttonClicked(ButtonPos position, MouseClick click)
{
	if (click == MouseClick::rightClick)
		presenter->buttonClicked(position, SurfaceClick::rightClick);
	else if (click == MouseClick::leftClick)
		presenter->buttonClicked(position, SurfaceClick::leftClick);

	ui.statusInfoLabel->setText(statuses[static_cast<int>(position)]);
}
