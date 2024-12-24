#include "NotificationDialog.h"
#include "ui_NotificationDialog.h"
#include <QPainter>

QStringList s_singular = {"Ден", "Месец", "Година"};
QStringList s_plural = {"Дни", "Месеца", "Години"};

NotificationDialog::NotificationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NotificationDialog)
{
    ui->setupUi(this);

    setWindowTitle("Добавяне на напомняне");

    for(auto& value : s_singular){
        ui->durationTypeCombo->addItem(value);
    }

    connect(ui->durationSpinValue, &QSpinBox::valueChanged, this, [&]{ recalculateDate(); });
    connect(ui->durationTypeCombo, &QComboBox::currentIndexChanged, this, [&]{ recalculateDate(); });

    connect(ui->okButton, &QPushButton::clicked, this, [&]{

        Notification n;
        n.description = ui->lineEdit->text().toStdString();

        if(n.description.empty()){

            ui->lineEdit->setFocus();
            return;
        }

        auto date = recalculateDate();

        n.date = Date(date.day(), date.month(), date.year());

        m_result = n;
        close();
    });

    ui->durationTypeCombo->setCurrentIndex(1);
}


QDate NotificationDialog::recalculateDate()
{
    auto value = ui->durationSpinValue->value();
    auto type = ui->durationTypeCombo->currentIndex();

    auto labels = value == 1 ? &s_singular : &s_plural;

    for(int i = 0; i < labels->size(); i++){

        ui->durationTypeCombo->setItemText(i, labels->at(i));
    }

    QDate result;

    switch(type){
        case 0: result = m_currentDate.addDays(value); break;
        case 1: result =  m_currentDate.addMonths(value); break;
        case 2: result =  m_currentDate.addYears(value); break;
    }

    ui->dateLabel->setText("Дата: " + result.toString("dd.MM.yyyy") + " г.");

    return result;
}

void NotificationDialog::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::white);
}

NotificationDialog::~NotificationDialog()
{
    delete ui;
}
