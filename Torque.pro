QT += core gui widgets

TEMPLATE = app

CONFIG += c++17
CONFIG -= qml_debug qtquickcompiler

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = torque

SRC_PATH = $$PWD/src

SOURCES += \
    $$SRC_PATH/main.cpp \
    $$SRC_PATH/Presenter/AllergiesDialog/AllergiesDialogPresenter.cpp \
    $$SRC_PATH/Presenter/PatientDialog/Reformator.cpp \
    $$SRC_PATH/Presenter/PatientDialog/PatientDialogPresenter.cpp \
    $$SRC_PATH/Presenter/ProcedureDialog/SubPresenters/EndoPresenter.cpp \
    $$SRC_PATH/Presenter/ProcedureDialog/SubPresenters/CrownPresenter.cpp \
    $$SRC_PATH/Presenter/ProcedureDialog/SubPresenters/ExtractionPresenter.cpp \
    $$SRC_PATH/Presenter/ProcedureDialog/SubPresenters/ImplantPresenter.cpp \
    $$SRC_PATH/Presenter/ProcedureDialog/SubPresenters/TeethMPresenter.cpp \
    $$SRC_PATH/Presenter/ProcedureDialog/SubPresenters/ObturationPresenter.cpp \
    $$SRC_PATH/Presenter/ProcedureDialog/SubPresenters/GeneralMPresenter.cpp \
    $$SRC_PATH/Presenter/ProcedureDialog/ProcedureDialogPresenter.cpp \
    $$SRC_PATH/Presenter/ListPresenter/SelectionManager.cpp \
    $$SRC_PATH/Presenter/ListPresenter/CheckModelCreator.cpp \
    $$SRC_PATH/Presenter/ListPresenter/ToothHintCreator.cpp \
    $$SRC_PATH/Presenter/ListPresenter/StatusControl.cpp \
    $$SRC_PATH/Presenter/ListPresenter/ListPresenter.cpp \
    $$SRC_PATH/Presenter/AmbListPagePresenter.cpp \
    $$SRC_PATH/Presenter/SurfacePanel/SurfacePanelPresenter.cpp \
    $$SRC_PATH/Presenter/SurfacePanel/ButtonSurfaceMatrix.cpp \
    $$SRC_PATH/Database/Database.cpp \
    $$SRC_PATH/Database/DbAmbList.cpp \
    $$SRC_PATH/Database/DbManipulation.cpp \
    $$SRC_PATH/Database/DbPatient.cpp \
    $$SRC_PATH/Model/Manipulation/getManipulationTamplates.cpp \
    $$SRC_PATH/Model/Manipulation/ManipulationApplier.cpp \
    $$SRC_PATH/Model/Manipulation/ManipulationParser.cpp \
    $$SRC_PATH/Model/Validator/ManipulationValidators.cpp \
    $$SRC_PATH/Model/Validator/AbstractValidator.cpp \
    $$SRC_PATH/Model/Validator/PatientValidators.cpp \
    $$SRC_PATH/Model/Tooth/ToothParser.cpp \
    $$SRC_PATH/Model/Tooth/ToothUtils.cpp \
    $$SRC_PATH/Model/Tooth/Tooth.cpp \
    $$SRC_PATH/Model/Tooth/ToothController/GeneralStatusControler.cpp \
    $$SRC_PATH/Model/Tooth/ToothController/BridgeController.cpp \
    $$SRC_PATH/Model/Tooth/ToothController/SurfaceController.cpp \
    $$SRC_PATH/Model/Tooth/ToothParserB.cpp \
    $$SRC_PATH/Model/CityCode.cpp \
    $$SRC_PATH/Model/ListInstance.cpp \
    $$SRC_PATH/Model/Date.cpp \
    $$SRC_PATH/View/uiComponents/qt_derived/Label.cpp \
    $$SRC_PATH/View/uiComponents/qt_derived/VitaWidget.cpp \
    $$SRC_PATH/View/uiComponents/qt_derived/ComboBox.cpp \
    $$SRC_PATH/View/uiComponents/qt_derived/CityLineEdit.cpp \
    $$SRC_PATH/View/uiComponents/qt_derived/SurfaceSelector.cpp \
    $$SRC_PATH/View/uiComponents/qt_derived/RangeWidget.cpp \
    $$SRC_PATH/View/uiComponents/qt_derived/LineEdit.cpp \
    $$SRC_PATH/View/uiComponents/qt_derived/DateLineEdit.cpp \
    $$SRC_PATH/View/uiComponents/qt_derived/TextEdit.cpp \
    $$SRC_PATH/View/uiComponents/qt_derived/SpinBox.cpp \
    $$SRC_PATH/View/uiComponents/qt_derived/IdLineEdit.cpp \
    $$SRC_PATH/View/uiComponents/AbstractLineEdit.cpp \
    $$SRC_PATH/View/uiComponents/AbstractUIElement.cpp \
    $$SRC_PATH/View/AllergiesDialog/AllergiesDialog.cpp \
    $$SRC_PATH/View/PatientDialog/PatientFormDialog.cpp \
    $$SRC_PATH/View/ProcedureDialog/ProcedureModel.cpp \
    $$SRC_PATH/View/ProcedureDialog/ProcedureDialog.cpp \
    $$SRC_PATH/View/ProcedureDialog/Widgets/CrownView/CrownView.cpp \
    $$SRC_PATH/View/ProcedureDialog/Widgets/CommonFields/CommonFields.cpp \
    $$SRC_PATH/View/ProcedureDialog/Widgets/ObturationView/ObturationView.cpp \
    $$SRC_PATH/View/ListView/Procedures/ProcedureTable.cpp \
    $$SRC_PATH/View/ListView/Procedures/ProcedureTableModel.cpp \
    $$SRC_PATH/View/ListView/ToothPaintDevices/ToothPainter.cpp \
    $$SRC_PATH/View/ListView/ToothPaintDevices/BridgePainter.cpp \
    $$SRC_PATH/View/ListView/ToothPaintDevices/SpriteSheets.cpp \
    $$SRC_PATH/View/ListView/ToothPaintDevices/SpriteRect.cpp \
    $$SRC_PATH/View/ListView/ToothPaintDevices/CPToothPainter.cpp \
    $$SRC_PATH/View/ListView/ControlPanel/ControlPanel.cpp \
    $$SRC_PATH/View/ListView/ControlPanel/StatusButton.cpp \
    $$SRC_PATH/View/ListView/tilebutton.cpp \
    $$SRC_PATH/View/ListView/ListView.cpp \
    $$SRC_PATH/View/ListView/TeethView/TeethViewScene.cpp \
    $$SRC_PATH/View/ListView/TeethView/ToothGraphicsItem.cpp \
    $$SRC_PATH/View/ListView/TeethView/ContextMenu.cpp \
    $$SRC_PATH/View/ListView/TeethView/SelectionBox.cpp \
    $$SRC_PATH/View/ListView/TeethView/GraphicsView.cpp \
    $$SRC_PATH/View/ListView/TeethView/BridgeItem.cpp \
    $$SRC_PATH/View/ListView/SurfacePanel/SurfacePanel.cpp \
    $$SRC_PATH/View/ListView/SurfacePanel/ControlPanelPolygon.cpp \
    $$SRC_PATH/View/ListView/SurfacePanel/CPTooth.cpp \
    $$SRC_PATH/View/Torque.cpp \
    $$SRC_PATH/View/AmbListPage/TabBar.cpp \
    $$SRC_PATH/View/AmbListPage/AmbListPage.cpp \
    $$SRC_PATH/View/AmbListPage/saveDialog/SaveDialog.cpp \
    $$SRC_PATH/View/AmbListPage/ScrollArea.cpp \
    $$SRC_PATH/View/AmbListPage/saveAsDialog/MnogoTypSpinBox.cpp \
    $$SRC_PATH/View/AmbListPage/saveAsDialog/SaveAsDialog.cpp


HEADERS += \
    $$SRC_PATH/Presenter/AllergiesDialog/AllergiesDialogPresenter.h \
    $$SRC_PATH/Presenter/PatientDialog/PatientDialogPresenter.h \
    $$SRC_PATH/Presenter/PatientDialog/Reformator.h \
    $$SRC_PATH/Presenter/PatientDialog/uiObserver.h \
    $$SRC_PATH/Presenter/ProcedureDialog/SubPresenters/TeethMPresenter.h \
    $$SRC_PATH/Presenter/ProcedureDialog/SubPresenters/EndoPresenter.h \
    $$SRC_PATH/Presenter/ProcedureDialog/SubPresenters/ObturationPresenter.h \
    $$SRC_PATH/Presenter/ProcedureDialog/SubPresenters/GeneralMPresenter.h \
    $$SRC_PATH/Presenter/ProcedureDialog/SubPresenters/ImplantPresenter.h \
    $$SRC_PATH/Presenter/ProcedureDialog/SubPresenters/CrownPresenter.h \
    $$SRC_PATH/Presenter/ProcedureDialog/SubPresenters/ExtractionPresenter.h \
    $$SRC_PATH/Presenter/ProcedureDialog/ProcedureDialogPresenter.h \
    $$SRC_PATH/Presenter/ListPresenter/ListPresenter.h \
    $$SRC_PATH/Presenter/ListPresenter/CheckModelCreator.h \
    $$SRC_PATH/Presenter/ListPresenter/StatusControl.h \
    $$SRC_PATH/Presenter/ListPresenter/IStatusControl.h \
    $$SRC_PATH/Presenter/ListPresenter/ToothHintCreator.h \
    $$SRC_PATH/Presenter/ListPresenter/SelectionManager.h \
    $$SRC_PATH/Presenter/AmbListPagePresenter.h \
    $$SRC_PATH/Presenter/SurfacePanel/SurfacePanelPresenter.h \
    $$SRC_PATH/Presenter/SurfacePanel/InputEnums.h \
    $$SRC_PATH/Presenter/SurfacePanel/ButtonSurfaceMatrix.h \
    $$SRC_PATH/Database/DbPatient.h \
    $$SRC_PATH/Database/sqLite/sqlite3ext.h \
    $$SRC_PATH/Database/sqLite/sqlite3.h \
    $$SRC_PATH/Database/AbstractORM.h \
    $$SRC_PATH/Database/DbAmbList.h \
    $$SRC_PATH/Database/Database.h \
    $$SRC_PATH/Database/DbManipulation.h \
    $$SRC_PATH/Model/CityCode.h \
    $$SRC_PATH/Model/Manipulation/getManipulationTamplates.h \
    $$SRC_PATH/Model/Manipulation/ManipulationParser.h \
    $$SRC_PATH/Model/Manipulation/Manipulation.h \
    $$SRC_PATH/Model/Manipulation/ManipulationTemplate.h \
    $$SRC_PATH/Model/Manipulation/ManipulationApplier.h \
    $$SRC_PATH/Model/Validator/ManipulationValidators.h \
    $$SRC_PATH/Model/Validator/AbstractValidator.h \
    $$SRC_PATH/Model/Validator/PatientValidators.h \
    $$SRC_PATH/Model/Tooth/ToothParserB.h \
    $$SRC_PATH/Model/Tooth/ToothParser.h \
    $$SRC_PATH/Model/Tooth/Status.h \
    $$SRC_PATH/Model/Tooth/Enums.h \
    $$SRC_PATH/Model/Tooth/ToothUtils.h \
    $$SRC_PATH/Model/Tooth/ToothController/SurfaceController.h \
    $$SRC_PATH/Model/Tooth/ToothController/GeneralStatusControler.h \
    $$SRC_PATH/Model/Tooth/ToothController/BridgeController.h \
    $$SRC_PATH/Model/Tooth/ToothController/ToothController.h \
    $$SRC_PATH/Model/Tooth/Vita.h \
    $$SRC_PATH/Model/Tooth/Tooth.h \
    $$SRC_PATH/Model/Tooth/SurfStatus.h \
    $$SRC_PATH/Model/AmbList.h \
    $$SRC_PATH/Model/CheckState.h \
    $$SRC_PATH/Model/ListInstance.h \
    $$SRC_PATH/Model/Date.h \
    $$SRC_PATH/Model/Patient.h \
    $$SRC_PATH/View/uiComponents/AbstractRangeEdit.h \
    $$SRC_PATH/View/uiComponents/AbstractUIElement.h \
    $$SRC_PATH/View/uiComponents/qt_derived/Label.h \
    $$SRC_PATH/View/uiComponents/qt_derived/ComboBox.h \
    $$SRC_PATH/View/uiComponents/qt_derived/RangeWidget.h \
    $$SRC_PATH/View/uiComponents/qt_derived/VitaWidget.h \
    $$SRC_PATH/View/uiComponents/qt_derived/SpinBox.h \
    $$SRC_PATH/View/uiComponents/qt_derived/TextEdit.h \
    $$SRC_PATH/View/uiComponents/qt_derived/CityLineEdit.h \
    $$SRC_PATH/View/uiComponents/qt_derived/DateLineEdit.h \
    $$SRC_PATH/View/uiComponents/qt_derived/IdLineEdit.h \
    $$SRC_PATH/View/uiComponents/qt_derived/SurfaceSelector.h \
    $$SRC_PATH/View/uiComponents/qt_derived/LineEdit.h \
    $$SRC_PATH/View/uiComponents/AbstractSpinBox.h \
    $$SRC_PATH/View/uiComponents/AbstractComboBox.h \
    $$SRC_PATH/View/uiComponents/AbstractLabel.h \
    $$SRC_PATH/View/uiComponents/AbstractSurfaceSelector.h \
    $$SRC_PATH/View/uiComponents/AbstractLineEdit.h \
    $$SRC_PATH/View/Torque.h \
    $$SRC_PATH/View/AllergiesDialog/IAllergiesDialog.h \
    $$SRC_PATH/View/AllergiesDialog/AllergiesDialog.h \
    $$SRC_PATH/View/PatientDialog/IPatientDialog.h \
    $$SRC_PATH/View/PatientDialog/PatientFormDialog.h \
    $$SRC_PATH/View/ProcedureDialog/ProcedureModel.h \
    $$SRC_PATH/View/ProcedureDialog/ProcedureDialog.h \
    $$SRC_PATH/View/ProcedureDialog/IProcedureDialog.h \
    $$SRC_PATH/View/ProcedureDialog/Widgets/CrownView/ICrownView.h \
    $$SRC_PATH/View/ProcedureDialog/Widgets/CrownView/CrownView.h \
    $$SRC_PATH/View/ProcedureDialog/Widgets/CommonFields/CommonFields.h \
    $$SRC_PATH/View/ProcedureDialog/Widgets/CommonFields/ICommonFields.h \
    $$SRC_PATH/View/ProcedureDialog/Widgets/ObturationView/IObturationView.h \
    $$SRC_PATH/View/ProcedureDialog/Widgets/ObturationView/ObturationView.h \
    $$SRC_PATH/View/ListView/Procedures/ProcedureTableModel.h \
    $$SRC_PATH/View/ListView/Procedures/ProcedureTable.h \
    $$SRC_PATH/View/ListView/Procedures/RowData.h \
    $$SRC_PATH/View/ListView/ToothPaintDevices/BridgePainter.h \
    $$SRC_PATH/View/ListView/ToothPaintDevices/PaintHint.h \
    $$SRC_PATH/View/ListView/ToothPaintDevices/ToothPainter.h \
    $$SRC_PATH/View/ListView/ToothPaintDevices/SpriteSheets.h \
    $$SRC_PATH/View/ListView/ToothPaintDevices/SpriteRect.h \
    $$SRC_PATH/View/ListView/ToothPaintDevices/CPToothPainter.h \
    $$SRC_PATH/View/ListView/ListView.h \
    $$SRC_PATH/View/ListView/ControlPanel/StatusButton.h \
    $$SRC_PATH/View/ListView/ControlPanel/ControlPanel.h \
    $$SRC_PATH/View/ListView/IListView.h \
    $$SRC_PATH/View/ListView/tilebutton.h \
    $$SRC_PATH/View/ListView/TeethView/TeethViewScene.h \
    $$SRC_PATH/View/ListView/TeethView/GraphicsView.h \
    $$SRC_PATH/View/ListView/TeethView/BridgeItem.h \
    $$SRC_PATH/View/ListView/TeethView/ContextMenu.h \
    $$SRC_PATH/View/ListView/TeethView/ToothGraphicsItem.h \
    $$SRC_PATH/View/ListView/TeethView/SelectionBox.h \
    $$SRC_PATH/View/ListView/SurfacePanel/ISurfacePanel.h \
    $$SRC_PATH/View/ListView/SurfacePanel/CPTooth.h \
    $$SRC_PATH/View/ListView/SurfacePanel/SurfacePanel.h \
    $$SRC_PATH/View/ListView/SurfacePanel/ControlPanelPolygon.h \
    $$SRC_PATH/View/AmbListPage/TabBar.h \
    $$SRC_PATH/View/AmbListPage/ScrollArea.h \
    $$SRC_PATH/View/AmbListPage/saveDialog/SaveDialog.h \
    $$SRC_PATH/View/AmbListPage/IAmbListPage.h \
    $$SRC_PATH/View/AmbListPage/AmbListPage.h \
    $$SRC_PATH/View/AmbListPage/saveAsDialog/MnogoTypSpinBox.h \
    $$SRC_PATH/View/AmbListPage/saveAsDialog/SaveAsDialog.h


FORMS += \
    $$SRC_PATH/View/uiComponents/qt_derived/RangeWidget.ui \
    $$SRC_PATH/View/uiComponents/qt_derived/VitaWidget.ui \
    $$SRC_PATH/View/uiComponents/qt_derived/SurfaceSelector.ui \
    $$SRC_PATH/View/AllergiesDialog/AllergiesDialog.ui \
    $$SRC_PATH/View/PatientDialog/PatientFormDialog.ui \
    $$SRC_PATH/View/ProcedureDialog/ProcedureDialog.ui \
    $$SRC_PATH/View/ProcedureDialog/Widgets/CrownView/CrownView.ui \
    $$SRC_PATH/View/ProcedureDialog/Widgets/CommonFields/CommonFields.ui \
    $$SRC_PATH/View/ProcedureDialog/Widgets/ObturationView/ObturationView.ui \
    $$SRC_PATH/View/Torque.ui \
    $$SRC_PATH/View/ListView/ListView.ui \
    $$SRC_PATH/View/ListView/ControlPanel/ControlPanel.ui \
    $$SRC_PATH/View/ListView/SurfacePanel/SurfacePanel.ui \
    $$SRC_PATH/View/AmbListPage/saveAsDialog/SaveAsDialog.ui \
    $$SRC_PATH/View/AmbListPage/AmbListPage.ui

RESOURCES += $$SRC_PATH/View/Torque.qrc

INCLUDEPATH += $$SRC_PATH #$$PWD/jsoncpp/jsoncpp-src-0.5.0/include

LIBS += -ljsoncpp -lsqlite3

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
