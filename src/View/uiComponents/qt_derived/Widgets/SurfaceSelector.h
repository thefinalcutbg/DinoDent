#pragma once

#include <QWidget>
#include "ui_SurfaceSelector.h"
#include "View/uiComponents/AbstractSurfaceSelector.h"

class QLabel;

class SurfaceSelector : public QWidget, public AbstractSurfaceSelector
{
	Q_OBJECT

private:
	QLabel* errorLabel;

public:
	SurfaceSelector(QWidget *parent = Q_NULLPTR);
	~SurfaceSelector();
	void setErrorLabel(QLabel* errorLabel);

	// Inherited via AbstractSurfaceSelector
	virtual void setFocus() override;
	virtual void disable(bool disable) override;
	virtual void setValidAppearence(bool valid) override;
	virtual std::array<bool, 6> getSurfaces() override;
	virtual void setSurfaces(const std::array<bool, 6>& surfaces);

private:
	Ui::SurfaceSelector ui;

};
