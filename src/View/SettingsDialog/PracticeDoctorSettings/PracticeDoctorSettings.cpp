#include "PracticeDoctorSettings.h"
#include "Presenter/SettingsPresenter/PracticeDoctorSettingsPresenter.h"
#include <QPainter>

void PracticeDoctorSettings::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(Qt::white));
}

PracticeDoctorSettings::PracticeDoctorSettings(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.adminCheck->setEnabled(false);

	ui.doctorList->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(ui.removeDoctor, &QPushButton::clicked, [=]{ presenter->deleteDoctor(); });
	connect(ui.addDoctor, &QPushButton::clicked, [=] {presenter->addDoctor();});
	connect(ui.doctorList, &QListWidget::currentRowChanged, [=](int row) {presenter->indexChanged(row);});
	connect(ui.adminCheck, &QCheckBox::stateChanged, [=](int state) { presenter->setAdminPrivilege(state);});
}

void PracticeDoctorSettings::setDoctorList(const std::vector<PracticeDoctor>& doctors)
{
	ui.doctorList->clear();

	for (auto doctor : doctors)
	{
		QString postfix = doctor.admin ? u8" (администратор)" : "";
		ui.doctorList->addItem(QString::fromStdString(doctor.name) + postfix);
	}
}

void PracticeDoctorSettings::setPresenter(PracticeDoctorSettingsPresenter* presenter)
{
	this->presenter = presenter;
}

void PracticeDoctorSettings::setAdminCheckbox(AdminStatus status)
{
	QSignalBlocker blocker(ui.adminCheck); //recursion guard

	switch (status)
	{
		case AdminStatus::Checked:
			ui.adminCheck->setEnabled(true);
			ui.adminCheck->setChecked(true);
			break;
		case AdminStatus::Unchecked:
			ui.adminCheck->setEnabled(true);
			ui.adminCheck->setChecked(false);
			break;
		case AdminStatus::Hidden:
			ui.adminCheck->setEnabled(false);
			ui.adminCheck->setChecked(false);
			break;
	}
}

void PracticeDoctorSettings::replaceCurrentItem(const PracticeDoctor& item)
{
	QString postfix = item.admin ? u8" (администратор)" : "";
	ui.doctorList->currentItem()->setText(QString::fromStdString(item.name) + postfix);
}

PracticeDoctorSettings::~PracticeDoctorSettings()
{
}
