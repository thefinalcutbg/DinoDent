#pragma once

#include <QWidget>
#include "ui_SurfaceSelector.h"
#include "View/uiComponents/AbstractSurfaceSelector.h"

class SurfaceSelector : public QWidget, public AbstractSurfaceSelector
{
	Q_OBJECT


public:
	SurfaceSelector(QWidget *parent = Q_NULLPTR);
	~SurfaceSelector();

	// Inherited via AbstractSurfaceSelector
	virtual void setFocusAndSelectAll() override;
	virtual void disable(bool disable) override;
	virtual void setAppearence(bool valid) override;
	virtual std::array<bool, 6> getSurfaces() override;
	virtual void setSurfaces(const std::array<bool, 6>& surfaces);

private:
	Ui::SurfaceSelector ui;

};
