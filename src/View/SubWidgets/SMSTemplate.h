#pragma once

#include <QWidget>
#include "ui_SMSTemplate.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SMSTemplateClass; };
QT_END_NAMESPACE

class SMSTemplate : public QWidget
{
	Q_OBJECT

	QString m_defaultTemplate;

public:
	SMSTemplate(QWidget *parent = nullptr);
	void setDefaultMessageTemplate(const QString& text);
	void setMessageTemplate(const QString& text);
	void setTitle(const QString& title);
	void setAutoCheck(bool autoCheck);
	bool getAutoCheck();
	QString getMessageTemplate() const;
	~SMSTemplate();

private:
	Ui::SMSTemplateClass *ui;
};

