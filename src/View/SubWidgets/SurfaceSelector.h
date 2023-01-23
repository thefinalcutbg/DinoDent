#pragma once

#include <QWidget>
#include "ui_SurfaceSelector.h"
#include "View/Interfaces/AbstractSurfaceSelector.h"

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
	virtual ProcedureObtData getData();
	virtual void setData(const ProcedureObtData& data);
	virtual void set_hidden(bool hidden) override { setHidden(hidden); }

private:
	Ui::SurfaceSelector ui;

};
