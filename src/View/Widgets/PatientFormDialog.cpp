#include "PatientFormDialog.h"
#include "Model/Country.h"

PatientFormDialog::PatientFormDialog(PatientDialogPresenter& p, QWidget* parent)
    : QDialog(parent),
    presenter(p)
{
    ui.setupUi(this);
    ui.hirbnoButton->setIcon(QIcon(":/icons/icon_nhif.png"));
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Нов документ");

    numValidator = new QRegularExpressionValidator(QRegularExpression("[0-9]+"), this);

    nameValidator = new QRegularExpressionValidator(QRegularExpression("[А-Я-а-я-a-z-A-Z ]+"), this);
    ui.fNameEdit->QLineEdit::setValidator(nameValidator);
    ui.mNameEdit->QLineEdit::setValidator(nameValidator);
    ui.lNameEdit->QLineEdit::setValidator(nameValidator);

    phoneValidator = new QRegularExpressionValidator(QRegularExpression("[0-9-+]+"), this);
    ui.phoneEdit->QLineEdit::setValidator(phoneValidator);

    ui.validDateEdit->set_Date(Date::currentDate());

    ui.cityLineEdit->setCompletions(Ekatte::cityNameToIdx());

    for (auto& country : Country::getCodeCountryPair())
    {
        ui.countryCombo->addItem(country.c_str());
    }
    //ui.countryCombo->setCurrentIndex(21); //BG default

    connect(ui.ehicRadio, &QRadioButton::toggled, this, [&] {
        ui.validDateLabel->setText("Валиден до:"); 
        ui.ehicLabel->setDisabled(false); 
        ui.ehic_edit->setDisabled(false); 
    });

    connect(ui.otherRadio, &QRadioButton::toggled, this, [&] {
        ui.validDateLabel->setText("Валидност от:"); 
        ui.ehicLabel->setDisabled(true);
        ui.ehic_edit->setDisabled(true);
    });

    connect(ui.typeComboBox, &QComboBox::currentIndexChanged, this,
        [&](int index) { presenter.changePatientType(index + 1); ui.idLineEdit->QLineEdit::setFocus(); });

    connect(ui.okButton, &QPushButton::clicked, this, [&] { presenter.accept(); });
    connect(ui.idLineEdit, &QLineEdit::textEdited, this, [&]{ if(ui.idLineEdit->isValid()) presenter.searchDbForPatient(ui.typeComboBox->currentIndex()+1); });
    connect(ui.hirbnoButton, &QPushButton::clicked, this, [&] { presenter.checkHirbno();});

    patientFields[id] = ui.idLineEdit;
    patientFields[fname] = ui.fNameEdit;
    patientFields[mname] = ui.mNameEdit;
    patientFields[lname] = ui.lNameEdit;
    patientFields[city] = ui.cityLineEdit;
    patientFields[address] = ui.addressEdit;
    patientFields[hirbno] = ui.HIRBNoEdit;
    patientFields[phone] = ui.phoneEdit;
    patientFields[foreign_city] = ui.foreignCityEdit;


    for (auto& line : patientFields)
    {
        line->setErrorLabel(ui.errorLabel);
    }

    ui.birthEdit->setErrorLabel(ui.errorLabel);

    presenter.setView(this);



}

void PatientFormDialog::paintEvent(QPaintEvent*)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
    painter.end();
}

PatientFormDialog::~PatientFormDialog()
{
}

void PatientFormDialog::setEditMode(bool editMode)
{
    ui.idLineEdit->setReadOnly(editMode);
    ui.typeComboBox->disable(editMode);
    ui.idLineEdit->setFocus();
    ui.idLineEdit->selectAll();

}

void PatientFormDialog::close()
{
    reject();
}

void PatientFormDialog::setType(Patient::Type type)
{

    ui.sexCombo->setCurrentIndex(0);
    ui.sexCombo->setDisabled(type == Patient::EGN);
    ui.birthEdit->setDisabled(type == Patient::EGN);
    ui.label_8->setDisabled(type == Patient::EGN);
    ui.label_7->setDisabled(type == Patient::EGN);
    

    ui.idLineEdit->setMaxLength(
        type < Patient::SSN ?
        10
        :
        20
    );

    ui.foreignerGroup->setHidden(type != Patient::EU);

}

void PatientFormDialog::setTitle(const std::string& title)
{
    setWindowTitle(title.c_str());
}

void PatientFormDialog::resetFields()
{
    ui.idLineEdit->reset();
    ui.idLineEdit->setValidAppearence(true);
    ui.birthEdit->reset();
    ui.fNameEdit->reset();
    ui.mNameEdit->reset();
    ui.lNameEdit->reset();
    ui.cityLineEdit->reset();
    ui.HIRBNoEdit->reset();
    ui.phoneEdit->reset();
    ui.addressEdit->reset();
    ui.foreignCityEdit->reset();

    ui.institutionNumber->clear();
    ui.ehic_edit->clear();

    ui.sexCombo->setCurrentIndex(0);
    ui.countryCombo->setCurrentIndex(0);

}

void PatientFormDialog::setPatient(const Patient& patient)
{
    QSignalBlocker b1(ui.idLineEdit);
    QSignalBlocker b2(ui.typeComboBox);
    QSignalBlocker b3(ui.cityLineEdit);

    ui.typeComboBox->setCurrentIndex(patient.type - 1);
    ui.idLineEdit->QLineEdit::setText(QString::fromStdString(patient.id));
    ui.sexCombo->setCurrentIndex(patient.sex);

    auto& date = patient.birth;
    ui.birthEdit->setDate(QDate(date.year, date.month, date.day));

    ui.fNameEdit->QLineEdit::setText(QString::fromStdString(patient.FirstName));
    ui.mNameEdit->QLineEdit::setText(QString::fromStdString(patient.MiddleName));
    ui.lNameEdit->QLineEdit::setText(QString::fromStdString(patient.LastName));

    ui.cityLineEdit->QLineEdit::setText(QString::fromStdString(patient.city.getString(false)));
    
    ui.addressEdit->QLineEdit::setText(QString::fromStdString(patient.address));
    ui.HIRBNoEdit->QLineEdit::setText(QString::fromStdString(patient.HIRBNo));
    ui.phoneEdit->QLineEdit::setText(QString::fromStdString(patient.phone));

    if (!patient.foreigner) return;
    
    ui.countryCombo->setCurrentIndex(patient.foreigner->country.getIndex());
    ui.institutionNumber->setText(patient.foreigner->institution.c_str());
    ui.validDateEdit->set_Date(patient.foreigner->date_valid);
    ui.ehic_edit->setText(patient.foreigner->ehic.c_str());
    ui.foreignCityEdit->setText(patient.foreigner->city.c_str());

    patient.foreigner->isEHIC() ?
        ui.ehicRadio->setChecked(true)
        :
        ui.otherRadio->setChecked(true)
    ;
}

Patient PatientFormDialog::getPatient()
{
    auto birthDate = ui.birthEdit->date();
    
    

    std::optional<Foreigner> f = ui.typeComboBox->currentIndex() == 3 ?

            Foreigner{
                .country = ui.countryCombo->currentIndex(),
                .city = ui.foreignCityEdit->getText(),
                .institution = ui.institutionNumber->text().toStdString(),
                .ehic = ui.ehicRadio->isChecked() ? ui.ehic_edit->text().toStdString() : std::string(),
                .date_valid = ui.validDateEdit->getDate(),
            }
            :
            std::optional<Foreigner>{}
    ;

    return Patient
    {
        .rowid = 0,
        .type = Patient::Type(ui.typeComboBox->currentIndex() + 1),
        .id = ui.idLineEdit->text().toStdString(),
        .birth = Date(birthDate.day(), birthDate.month(), birthDate.year()),
        .sex = Patient::Sex(ui.sexCombo->currentIndex()),
        .FirstName = ui.fNameEdit->text().toStdString(),
        .MiddleName = ui.mNameEdit->text().toStdString(),
        .LastName = ui.lNameEdit->text().toStdString(),
        .city = ui.cityLineEdit->text().toStdString(),
        .address = ui.addressEdit->text().toStdString(),
        .HIRBNo = ui.HIRBNoEdit->text().toStdString(),
        .phone = ui.phoneEdit->text().toStdString(),
        .foreigner = f
    };
}

void PatientFormDialog::setHirbno(const std::string& hirbno)
{
    ui.HIRBNoEdit->setText(hirbno.data());
    ui.HIRBNoEdit->setFocus();
    ui.HIRBNoEdit->selectAll();
}


AbstractLineEdit* PatientFormDialog::lineEdit(PatientField field)
{
    return patientFields[field];
}

AbstractDateEdit* PatientFormDialog::dateEdit()
{
    return ui.birthEdit;
}


