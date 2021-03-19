#include "patientformdialog.h"

PatientFormDialog::PatientFormDialog(Database *database, QWidget* parent)
	: QDialog(parent), 
	presenter(this, database)
{
	ui.setupUi(this);
	setModal(true);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle("Нов амбулаторен лист");

	numValidator = new QRegExpValidator(QRegExp(("[0-9]+")), this);
	ui.idLineEdit->setValidator(numValidator);
	ui.HIRBNoEdit->setValidator(numValidator);

	nameValidator = new QRegExpValidator(QRegExp(("[А-Я-а-я-a-z-A-Z- ]+")), this);
	ui.fNameEdit->setValidator(nameValidator);
	ui.mNameEdit->setValidator(nameValidator);
	ui.lNameEdit->setValidator(nameValidator);

	phoneValidator = new QRegExpValidator(QRegExp("[0-9-+ ]+"), this);
	ui.phoneEdit->setValidator(phoneValidator);

	connect(ui.typeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[=](int index) 
		{
			if (index)presenter.Ln4TypeDialog();
			else presenter.EgnTypeDialog();
		});

	connect(ui.okButton, &QPushButton::clicked, [=] { presenter.accept(); });

	std::array<AbstractLineEdit*, 9> patientFields;
	patientFields[id] = ui.idLineEdit;
	patientFields[birthdate] = ui.birthEdit;
	patientFields[fname] = ui.fNameEdit;
	patientFields[mname] = ui.mNameEdit;
	patientFields[lname] = ui.lNameEdit;
	patientFields[city] = ui.cityLineEdit;
	patientFields[address] = ui.addressEdit;
	patientFields[hirbno] = ui.HIRBNoEdit;
	patientFields[phone] = ui.phoneEdit;
	
	presenter.setPatientFields(patientFields, ui.typeComboBox, ui.sexCombo, ui.codeLabel);

}

void PatientFormDialog::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
	painter.end();
}

PatientFormDialog::~PatientFormDialog()
{

}

void PatientFormDialog::open(bool editMode)
{
	if (editMode)
	{
		ui.idLineEdit->disable(1);
		ui.typeComboBox->disable(1);
		setWindowTitle("Редактиране на данните на пациента");
	}
	else
	{
		ui.idLineEdit->disable(0);
		ui.typeComboBox->disable(0);
		setWindowTitle("Нов амбулаторен лист");
	}
	this->exec();
}

void PatientFormDialog::close()
{
	reject();
}

void PatientFormDialog::setLn4View(bool show)
{
	if(show)
	{
		ui.birthEdit->show();
		ui.sexCombo->show();
		ui.label_7->show();
		ui.label_8->show();
	}
	else
	{
		ui.birthEdit->hide();
		ui.sexCombo->hide();
		ui.label_7->hide();
		ui.label_8->hide();
	}
}

void PatientFormDialog::accept()
{
	presenter.accept();
}


PatientDialogPresenter* PatientFormDialog::Presenter()
{ 
	return &this->presenter;
}
