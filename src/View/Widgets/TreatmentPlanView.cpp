#include "TreatmentPlanView.h"
#include "ui_TreatmentPlanView.h"
#include "Presenter/TreatmentPlanPresenter.h"
#include "View/Graphics/TeethViewScene.h"
#include "View/GlobalFunctions.h"

#include "View/Theme.h"
#include <QPainter>
#include <QPainterPath>
#include <QTreeWidgetItem>
#include <QScrollBar>

TreatmentPlanView::TreatmentPlanView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TreatmentPlanView)
{
    ui->setupUi(this);

    //ui->stageList->setWordWrap(true);

    ui->stageList->setHeaderHidden(true);
    ui->stageList->header()->setStretchLastSection(false);
    ui->stageList->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->stageList->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);


    ui->addProcedure->setIcon(QIcon(":/icons/icon_add.png"));
    ui->addStage->setIcon(QIcon(":/icons/icon_add.png"));
    ui->addConclusion->setIcon(QIcon(":/icons/icon_add.png"));
    ui->deleteButton->setIcon(QIcon(":/icons/icon_remove.png"));
    ui->editButton->setIcon(QIcon(":/icons/icon_edit.png"));

    ui->addProcedure->setHoverColor(Theme::mainBackgroundColor);
    ui->addStage->setHoverColor(Theme::mainBackgroundColor);
    ui->addConclusion->setHoverColor(Theme::mainBackgroundColor);
    ui->deleteButton->setHoverColor(Theme::mainBackgroundColor);
    ui->editButton->setHoverColor(Theme::mainBackgroundColor);

    teethViewScene = new TeethViewScene(ui->teethView);
    ui->teethView->setScene(teethViewScene);
    ui->teethView->setSceneRect(teethViewScene->sceneRect());

    connect(ui->stageList, &QTreeWidget::itemSelectionChanged, this, [=, this] {

            if(presenter) presenter->selectionChanged(getSelection());
    });

    connect(ui->addStage, &QPushButton::clicked, this, [=, this] { if(presenter) presenter->addStage(); });
    connect(ui->deleteButton, &QPushButton::clicked, this, [=, this] { if(presenter) presenter->removePressed(); });
    connect(ui->editButton, &QPushButton::clicked, this, [=, this] { if(presenter) presenter->editPressed(); });
    connect(ui->completedCheck, &QCheckBox::clicked, this, [=, this](bool checked) {
        disableEditFileds(checked);
        if(presenter) presenter->setCompleted(checked);
    });

    connect(ui->addProcedure, &QPushButton::clicked, this, [=, this] {;

        if(presenter)presenter->addProcedure(teethViewScene->getSelectedTeethIdx());
    });

    connect(teethViewScene, &TeethViewScene::toothDoubleClicked, [&, this]{ ui->addProcedure->click();});

    connect(ui->stageList, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem *item, int column){

        if(!presenter|| !item) return;

            switch(column){
            case 0: presenter->nameEditRequested(); break;
            case 1: presenter->priceEditRequested(); break;
        }
    });

    connect(ui->dateEdit, &QDateEdit::dateChanged, this, [=, this](const QDate& date){ if(presenter) presenter->dateChanged(Date(date)); });

    setStyleSheet(Theme::getFancyStylesheet());
}

void TreatmentPlanView::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::RenderHint::Antialiasing);
    painter.fillRect(rect(), Theme::background);

    QPainterPath path;

    path.addRoundedRect(
        ui->frameOut->x(),
        ui->frameOut->y(),
        ui->frameOut->width(),
        ui->frameOut->height(),
        Theme::radius/2,
        Theme::radius/2
        );

    painter.fillPath(path, Theme::sectionBackground);

    painter.setPen(QPen(Theme::mainBackgroundColor));
    painter.drawPath(path);

    painter.drawLine(
        ui->frameOut->x() + ui->teethView->width(),
        ui->frameOut->y(),  //y1
        ui->frameOut->x() + ui->teethView->width(),
        ui->frameOut->y()+ui->teethView->height()
        );

}

std::pair<int, int> TreatmentPlanView::TreatmentPlanView::getSelection() const
{
    auto *item = ui->stageList->currentItem();
    if (!item)
        return {-1, -1};

    QTreeWidgetItem *parent = item->parent();

    if (!parent) {
        const int stageIndex = ui->stageList->indexOfTopLevelItem(item);
        return {stageIndex, -1};
    }

    return {ui->stageList->indexOfTopLevelItem(parent), parent->indexOfChild(item)};
}

void TreatmentPlanView::disableEditFileds(bool disabled)
{
    ui->dateEdit->setDisabled(disabled);
    ui->editButton->setHidden(disabled);
    ui->deleteButton->setHidden(disabled);
    ui->addProcedure->setHidden(disabled);
    ui->addStage->setHidden(disabled);
    ui->addConclusion->setHidden(disabled);
}

void TreatmentPlanView::setPresenter(TreatmentPlanPresenter *p)
{
    presenter = p;
}

void TreatmentPlanView::repaintTooth(const ToothPaintHint &tooth)
{
    teethViewScene->display(tooth);
}

void TreatmentPlanView::setTreatmentPlan(const TreatmentPlan &p)
{
    QSignalBlocker b1(ui->completedCheck);

    ui->completedCheck->setChecked(p.is_completed);

    disableEditFileds(p.is_completed);

    QSignalBlocker b2(ui->stageList);

    ui->dateEdit->set_Date(p.date);

    ui->stageList->clear();

    static const QString numArr[] = {
        "Първи", "Втори", "Трети", "Четвърти", "Пети",
        "Шести", "Седми", "Осми", "Девети", "Десети"
    };

    //setting stages

    for(int i = 0; i < p.stages.size(); i++){

        const auto &stage = p.stages[i];

        auto stageText = "<b>" + numArr[i] + " етап</b>";

        if(stage.notes.size()){
            stageText += " - ";
            stageText += QString::fromStdString(stage.notes);
        }

        auto *stageItem = new QTreeWidgetItem(ui->stageList);

        auto *label = new QLabel;
        label->setTextFormat(Qt::RichText);
        label->setText(stageText);

        stageItem->setText(1, priceRangeToString(stage.getPriceLabel()));

        stageItem->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);

        ui->stageList->setItemWidget(stageItem, 0, label);

        //setting procedures

        for (int j = 0; j < static_cast<int>(stage.plannedProcedures.size()); ++j)
        {
            const auto &procedure = stage.plannedProcedures[j];

            auto *procItem = new QTreeWidgetItem(stageItem);
            procItem->setText(0, procedure.getName().c_str());
            procItem->setText(1, priceRangeToString(procedure.priceRange));

            if(procedure.isCompleted){
                procItem->setIcon(0, CommonIcon::getPixmap(CommonIcon::CHECKED));
            }

            procItem->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);

            if (!procedure.notes.empty())
                procItem->setToolTip(0, QString::fromStdString(procedure.notes));
        }

        stageItem->setExpanded(true);
    }
}

void TreatmentPlanView::setAffectedTeeth(const std::vector<int>& indexes)
{
    teethViewScene->setProcedures(indexes);
}

void TreatmentPlanView::setSelection(const std::pair<int, int>& pair)
{
    QSignalBlocker b(ui->stageList);

    ui->stageList->clearSelection();

    if (pair.first == -1){ return;}

    auto stageItem = ui->stageList->topLevelItem(pair.first);

    ui->stageList->scrollToItem(stageItem, QAbstractItemView::PositionAtCenter);

    stageItem->setSelected(true);

    if(pair.second == -1) { return; }

    stageItem->setSelected(false);

    auto procedureItem = stageItem->child(pair.second);

    stageItem->setExpanded(true);

    procedureItem->setSelected(true);

    ui->stageList->scrollToItem(procedureItem, QAbstractItemView::PositionAtCenter);
}

PatientTileInfo *TreatmentPlanView::tileInfo()
{
    return ui->patientInfoTile;
}

TreatmentPlanView::~TreatmentPlanView()
{
    delete ui;
}
