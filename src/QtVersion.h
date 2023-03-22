#pragma once
#include <QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)

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