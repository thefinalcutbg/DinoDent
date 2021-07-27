QT += core gui widgets

TEMPLATE = app

CONFIG += c++17
CONFIG -= qml_debug qtquickcompiler

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = torque

SRC_PATH = $$PWD/src
INCLUDEPATH += $$PWD/src

SOURCES += \
    src/Database/Database.cpp \
    src/Database/DbAmbList.cpp \
    src/Database/DbPatient.cpp \
    src/Database/DbProcedure.cpp \
    src/Database/sqLite/shell.c \
    src/Database/sqLite/sqlite3.c \
    src/JsonCpp/jsoncpp.cpp \
    src/Model/AmbListValidator.cpp \
    src/Model/CityCode.cpp \
    src/Model/Date.cpp \
    src/Model/Patient.cpp \
    src/Model/Procedure/CustomProcedures.cpp \
    src/Model/Procedure/MasterNZOK.cpp \
    src/Model/Procedure/PackageCounter.cpp \
    src/Model/Procedure/ProcedureApplier.cpp \
    src/Model/Procedure/ProcedureParser.cpp \
    src/Model/Tooth/Tooth.cpp \
    src/Model/Tooth/ToothController/BridgeController.cpp \
    src/Model/Tooth/ToothController/GeneralStatusControler.cpp \
    src/Model/Tooth/ToothController/SurfaceController.cpp \
    src/Model/Tooth/ToothParser.cpp \
    src/Model/Tooth/ToothUtils.cpp \
    src/Model/User/User.cpp \
    src/Model/Validator/CommonValidators.cpp \
    src/Model/Validator/DateValidator.cpp \
    src/Model/Validator/NameValidator.cpp \
    src/Model/Validator/PatientValidators.cpp \
    src/Model/Validator/ProcedureDateValidator.cpp \
    src/Model/Validator/ProcedureValidators.cpp \
    src/Presenter/AllergiesDialog/AllergiesDialogPresenter.cpp \
    src/Presenter/AmbListPagePresenter.cpp \
    src/Presenter/DetailsPresenter/DetailsPresenter.cpp \
    src/Presenter/ListPresenter/Editor.cpp \
    src/Presenter/ListPresenter/ListPresenter.cpp \
    src/Presenter/ListPresenter/ProcedurePresenter/ProcedurePresenter.cpp \
    src/Presenter/ListPresenter/StatusPresenter/CheckModelCreator.cpp \
    src/Presenter/ListPresenter/StatusPresenter/StatusControl.cpp \
    src/Presenter/ListPresenter/StatusPresenter/StatusPresenter.cpp \
    src/Presenter/ListPresenter/StatusPresenter/SurfacePanel/ButtonSurfaceMatrix.cpp \
    src/Presenter/ListPresenter/StatusPresenter/SurfacePanel/SurfacePanelPresenter.cpp \
    src/Presenter/ListPresenter/StatusPresenter/ToothHintCreator.cpp \
    src/Presenter/ListSelector/ListSelectorPresenter.cpp \
    src/Presenter/PatientDialog/PatientDialogPresenter.cpp \
    src/Presenter/ProcedureDialog/ProcedureDialogPresenter.cpp \
    src/Presenter/ProcedureDialog/ProcedureEditorPresenter.cpp \
    src/Presenter/ProcedureDialog/SubPresenters/CrownPresenter.cpp \
    src/Presenter/ProcedureDialog/SubPresenters/EndoPresenter.cpp \
    src/Presenter/ProcedureDialog/SubPresenters/ExtractionPresenter.cpp \
    src/Presenter/ProcedureDialog/SubPresenters/GeneralMPresenter.cpp \
    src/Presenter/ProcedureDialog/SubPresenters/ImplantPresenter.cpp \
    src/Presenter/ProcedureDialog/SubPresenters/ObturationPresenter.cpp \
    src/Presenter/ProcedureDialog/SubPresenters/TeethMPresenter.cpp \
    src/Presenter/TabPresenter/ListInstance.cpp \
    src/Presenter/TabPresenter/TabPresenter.cpp \
    src/View/AllergiesDialog/AllergiesDialog.cpp \
    src/View/AmbListPage/AmbListPage.cpp \
    src/View/AmbListPage/TabView/TabBar.cpp \
    src/View/AmbListPage/TabView/TabView.cpp \
    src/View/AmbListPage/saveAsDialog/MnogoTypSpinBox.cpp \
    src/View/AmbListPage/saveAsDialog/SaveAsDialog.cpp \
    src/View/AmbListPage/saveDialog/SaveDialog.cpp \
    src/View/AmbListSelector/AmbListSelector.cpp \
    src/View/AmbListSelector/ListTable.cpp \
    src/View/AmbListSelector/ListsTableModel.cpp \
    src/View/DetailsView/DetailsView.cpp \
    src/View/ErrorMessage.cpp \
    src/View/GlobalFunctions.cpp \
    src/View/ListView/ControlPanel/ControlPanel.cpp \
    src/View/ListView/ControlPanel/StatusButton.cpp \
    src/View/ListView/ListView.cpp \
    src/View/ListView/Procedures/ProcedureTable.cpp \
    src/View/ListView/Procedures/ProcedureTableModel.cpp \
    src/View/ListView/SurfacePanel/CPTooth.cpp \
    src/View/ListView/SurfacePanel/ControlPanelPolygon.cpp \
    src/View/ListView/SurfacePanel/SurfacePanel.cpp \
    src/View/ListView/TeethView/BridgeItem.cpp \
    src/View/ListView/TeethView/ContextMenu.cpp \
    src/View/ListView/TeethView/GraphicsView.cpp \
    src/View/ListView/TeethView/SelectionBox.cpp \
    src/View/ListView/TeethView/TeethViewScene.cpp \
    src/View/ListView/TeethView/ToothGraphicsItem.cpp \
    src/View/ListView/ToothPaintDevices/BridgePainter.cpp \
    src/View/ListView/ToothPaintDevices/CPToothPainter.cpp \
    src/View/ListView/ToothPaintDevices/SpriteRect.cpp \
    src/View/ListView/ToothPaintDevices/SpriteSheets.cpp \
    src/View/ListView/ToothPaintDevices/ToothPainter.cpp \
    src/View/ListView/ToothPaintDevices/ToothSprite.cpp \
    src/View/ListView/tilebutton.cpp \
    src/View/ModalDialogBuilder.cpp \
    src/View/PatientDialog/PatientFormDialog.cpp \
    src/View/ProcedureDialog/ProcedureDialog.cpp \
    src/View/ProcedureDialog/ProcedureModel.cpp \
    src/View/ProcedureDialog/Widgets/CommonFields/CommonFields.cpp \
    src/View/ProcedureDialog/Widgets/CrownView/CrownView.cpp \
    src/View/ProcedureDialog/Widgets/ImplantView/ImplantView.cpp \
    src/View/ProcedureDialog/Widgets/ObturationView/ObturationView.cpp \
    src/View/ProcedureEditDialog/ProcedureEditDialog.cpp \
    src/View/Torque.cpp \
    src/View/uiComponents/AbstractDateEdit.cpp \
    src/View/uiComponents/AbstractLineEdit.cpp \
    src/View/uiComponents/AbstractUIElement.cpp \
    src/View/uiComponents/qt_derived/CityLineEdit.cpp \
    src/View/uiComponents/qt_derived/ComboBox.cpp \
    src/View/uiComponents/qt_derived/DateEdit.cpp \
    src/View/uiComponents/qt_derived/LineEdit.cpp \
    src/View/uiComponents/qt_derived/NameEdit.cpp \
    src/View/uiComponents/qt_derived/RangeWidget.cpp \
    src/View/uiComponents/qt_derived/SpinBox.cpp \
    src/View/uiComponents/qt_derived/SurfaceSelector.cpp \
    src/View/uiComponents/qt_derived/TextEdit.cpp \
    src/View/uiComponents/qt_derived/VitaWidget.cpp \
    src/main.cpp



HEADERS += \
    src/Database/AbstractORM.h \
    src/Database/Database.h \
    src/Database/DbAmbList.h \
    src/Database/DbPatient.h \
    src/Database/DbProcedure.h \
    src/Database/sqLite/sqlite3.h \
    src/Database/sqLite/sqlite3ext.h \
    src/JsonCpp/json-forwards.h \
    src/JsonCpp/json.h \
    src/Model/AmbList.h \
    src/Model/AmbListRow.h \
    src/Model/AmbListValidator.h \
    src/Model/CheckState.h \
    src/Model/CityCode.h \
    src/Model/Date.h \
    src/Model/Patient.h \
    src/Model/Procedure/CustomProcedures.h \
    src/Model/Procedure/MasterNZOK.h \
    src/Model/Procedure/NZOKmaps.h \
    src/Model/Procedure/PackageCounter.h \
    src/Model/Procedure/Procedure.h \
    src/Model/Procedure/ProcedureApplier.h \
    src/Model/Procedure/ProcedureParser.h \
    src/Model/Procedure/ProcedureTemplate.h \
    src/Model/Tooth/Enums.h \
    src/Model/Tooth/Status.h \
    src/Model/Tooth/SurfStatus.h \
    src/Model/Tooth/Tooth.h \
    src/Model/Tooth/ToothController/BridgeController.h \
    src/Model/Tooth/ToothController/GeneralStatusControler.h \
    src/Model/Tooth/ToothController/SurfaceController.h \
    src/Model/Tooth/ToothController/ToothController.h \
    src/Model/Tooth/ToothParser.h \
    src/Model/Tooth/ToothUtils.h \
    src/Model/Tooth/Vita.h \
    src/Model/User/User.h \
    src/Model/Validator/AbstractValidator.h \
    src/Model/Validator/CommonValidators.h \
    src/Model/Validator/DateValidator.h \
    src/Model/Validator/NameValidator.h \
    src/Model/Validator/PatientValidators.h \
    src/Model/Validator/ProcedureDateValidator.h \
    src/Model/Validator/ProcedureValidators.h \
    src/Presenter/AllergiesDialog/AllergiesDialogPresenter.h \
    src/Presenter/AmbListPagePresenter.h \
    src/Presenter/DetailsPresenter/DetailsPresenter.h \
    src/Presenter/ListPresenter/Editor.h \
    src/Presenter/ListPresenter/ListPresenter.h \
    src/Presenter/ListPresenter/ProcedurePresenter/ProcedurePresenter.h \
    src/Presenter/ListPresenter/StatusPresenter/CheckModelCreator.h \
    src/Presenter/ListPresenter/StatusPresenter/InputEnums.h \
    src/Presenter/ListPresenter/StatusPresenter/StatusControl.h \
    src/Presenter/ListPresenter/StatusPresenter/StatusPresenter.h \
    src/Presenter/ListPresenter/StatusPresenter/SurfacePanel/ButtonSurfaceMatrix.h \
    src/Presenter/ListPresenter/StatusPresenter/SurfacePanel/SurfacePanelPresenter.h \
    src/Presenter/ListPresenter/StatusPresenter/ToothHintCreator.h \
    src/Presenter/ListSelector/ListSelectorPresenter.h \
    src/Presenter/PatientDialog/PatientDialogPresenter.h \
    src/Presenter/ProcedureDialog/ProcedureDialogPresenter.h \
    src/Presenter/ProcedureDialog/ProcedureEditorPresenter.h \
    src/Presenter/ProcedureDialog/SubPresenters/CrownPresenter.h \
    src/Presenter/ProcedureDialog/SubPresenters/EndoPresenter.h \
    src/Presenter/ProcedureDialog/SubPresenters/ExtractionPresenter.h \
    src/Presenter/ProcedureDialog/SubPresenters/GeneralMPresenter.h \
    src/Presenter/ProcedureDialog/SubPresenters/ImplantPresenter.h \
    src/Presenter/ProcedureDialog/SubPresenters/ObturationPresenter.h \
    src/Presenter/ProcedureDialog/SubPresenters/TeethMPresenter.h \
    src/Presenter/TabPresenter/ListInstance.h \
    src/Presenter/TabPresenter/TabPresenter.h \
    src/View/AllergiesDialog/AllergiesDialog.h \
    src/View/AllergiesDialog/IAllergiesDialog.h \
    src/View/AmbListPage/AmbListPage.h \
    src/View/AmbListPage/IAmbListPage.h \
    src/View/AmbListPage/TabView/ITabView.h \
    src/View/AmbListPage/TabView/TabBar.h \
    src/View/AmbListPage/TabView/TabView.h \
    src/View/AmbListPage/saveAsDialog/MnogoTypSpinBox.h \
    src/View/AmbListPage/saveAsDialog/SaveAsDialog.h \
    src/View/AmbListPage/saveDialog/SaveDialog.h \
    src/View/AmbListSelector/AmbListSelector.h \
    src/View/AmbListSelector/IListSelectorView.h \
    src/View/AmbListSelector/ListTable.h \
    src/View/AmbListSelector/ListsTableModel.h \
    src/View/DetailsView/DetailsView.h \
    src/View/DetailsView/IDetailsView.h \
    src/View/ErrorMessage.h \
    src/View/GlobalFunctions.h \
    src/View/ListView/ControlPanel/ControlPanel.h \
    src/View/ListView/ControlPanel/StatusButton.h \
    src/View/ListView/IListView.h \
    src/View/ListView/IProcedureView.h \
    src/View/ListView/IStatusView.h \
    src/View/ListView/ListView.h \
    src/View/ListView/Procedures/ProcedureTable.h \
    src/View/ListView/Procedures/ProcedureTableModel.h \
    src/View/ListView/Procedures/RowData.h \
    src/View/ListView/SurfacePanel/CPTooth.h \
    src/View/ListView/SurfacePanel/ControlPanelPolygon.h \
    src/View/ListView/SurfacePanel/ISurfacePanel.h \
    src/View/ListView/SurfacePanel/SurfacePanel.h \
    src/View/ListView/TeethView/BridgeItem.h \
    src/View/ListView/TeethView/ContextMenu.h \
    src/View/ListView/TeethView/GraphicsView.h \
    src/View/ListView/TeethView/SelectionBox.h \
    src/View/ListView/TeethView/TeethViewScene.h \
    src/View/ListView/TeethView/ToothGraphicsItem.h \
    src/View/ListView/ToothPaintDevices/BridgePainter.h \
    src/View/ListView/ToothPaintDevices/CPToothPainter.h \
    src/View/ListView/ToothPaintDevices/PaintHint.h \
    src/View/ListView/ToothPaintDevices/SpriteRect.h \
    src/View/ListView/ToothPaintDevices/SpriteSheets.h \
    src/View/ListView/ToothPaintDevices/ToothPainter.h \
    src/View/ListView/ToothPaintDevices/ToothSprite.h \
    src/View/ListView/tilebutton.h \
    src/View/ModalDialogBuilder.h \
    src/View/PatientDialog/IPatientDialog.h \
    src/View/PatientDialog/PatientFormDialog.h \
    src/View/ProcedureDialog/IProcedureDialog.h \
    src/View/ProcedureDialog/ProcedureDialog.h \
    src/View/ProcedureDialog/ProcedureModel.h \
    src/View/ProcedureDialog/Widgets/CommonFields/CommonFields.h \
    src/View/ProcedureDialog/Widgets/CommonFields/ICommonFields.h \
    src/View/ProcedureDialog/Widgets/CrownView/CrownView.h \
    src/View/ProcedureDialog/Widgets/CrownView/ICrownView.h \
    src/View/ProcedureDialog/Widgets/ImplantView/IImplantView.h \
    src/View/ProcedureDialog/Widgets/ImplantView/ImplantView.h \
    src/View/ProcedureDialog/Widgets/ObturationView/IObturationView.h \
    src/View/ProcedureDialog/Widgets/ObturationView/ObturationView.h \
    src/View/ProcedureEditDialog/IProcedureEditDialog.h \
    src/View/ProcedureEditDialog/ProcedureEditDialog.h \
    src/View/Torque.h \
    src/View/uiComponents/AbstractComboBox.h \
    src/View/uiComponents/AbstractDateEdit.h \
    src/View/uiComponents/AbstractLabel.h \
    src/View/uiComponents/AbstractLineEdit.h \
    src/View/uiComponents/AbstractRangeEdit.h \
    src/View/uiComponents/AbstractSpinBox.h \
    src/View/uiComponents/AbstractSurfaceSelector.h \
    src/View/uiComponents/AbstractUIElement.h \
    src/View/uiComponents/qt_derived/CityLineEdit.h \
    src/View/uiComponents/qt_derived/ComboBox.h \
    src/View/uiComponents/qt_derived/DateEdit.h \
    src/View/uiComponents/qt_derived/LineEdit.h \
    src/View/uiComponents/qt_derived/NameEdit.h \
    src/View/uiComponents/qt_derived/RangeWidget.h \
    src/View/uiComponents/qt_derived/SpinBox.h \
    src/View/uiComponents/qt_derived/SurfaceSelector.h \
    src/View/uiComponents/qt_derived/TextEdit.h \
    src/View/uiComponents/qt_derived/VitaWidget.h



FORMS += \
    src/View/AllergiesDialog/AllergiesDialog.ui \
    src/View/AmbListPage/AmbListPage.ui \
    src/View/AmbListPage/TabView/TabView.ui \
    src/View/AmbListPage/saveAsDialog/SaveAsDialog.ui \
    src/View/AmbListSelector/AmbListSelector.ui \
    src/View/DetailsView/DetailsView.ui \
    src/View/ListView/ControlPanel/ControlPanel.ui \
    src/View/ListView/ListView.ui \
    src/View/ListView/SurfacePanel/SurfacePanel.ui \
    src/View/PatientDialog/PatientFormDialog.ui \
    src/View/ProcedureDialog/ProcedureDialog.ui \
    src/View/ProcedureDialog/Widgets/CommonFields/CommonFields.ui \
    src/View/ProcedureDialog/Widgets/CrownView/CrownView.ui \
    src/View/ProcedureDialog/Widgets/ImplantView/ImplantView.ui \
    src/View/ProcedureDialog/Widgets/ObturationView/ObturationView.ui \
    src/View/ProcedureEditDialog/ProcedureEditDialog.ui \
    src/View/Torque.ui \
    src/View/uiComponents/qt_derived/RangeWidget.ui \
    src/View/uiComponents/qt_derived/SurfaceSelector.ui \
    src/View/uiComponents/qt_derived/VitaWidget.ui


RESOURCES += $$SRC_PATH/View/Torque.qrc \
    src/View/Torque.qrc \
    src/View/Torque.qrc


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
