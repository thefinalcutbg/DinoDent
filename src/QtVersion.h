#pragma once
#include <QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#define QT5
#else
#define QT6
#endif

#ifdef QT5

#define QtComboIndexChanged QOverload<int>::of(&QComboBox::currentIndexChanged)
#define QtDoubleSpinValueChanged QOverload<double>::of(&QDoubleSpinBox::valueChanged)
#define QtSpinValueChanged QOverload<int>::of(&QSpinBox::valueChanged)
#define QtIODevice QIODevice

#else

#define QtComboIndexChanged &QComboBox::currentIndexChanged
#define QtDoubleSpinValueChanged &QDoubleSpinBox::valueChanged
#define QtSpinValueChanged &QSpinBox::valueChanged
#define QtIODevice QIODeviceBase

#endif

#ifdef Q_PROCESSOR_X86_32 
#define IS32BIT
#else
#define IS46BIT
#endif