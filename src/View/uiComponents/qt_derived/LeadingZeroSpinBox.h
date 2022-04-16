#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QAbstractSpinBox>
#include <QtWidgets/QLineEdit>

class LeadingZeroSpinBox : public QAbstractSpinBox
{
    Q_OBJECT
public:
    explicit LeadingZeroSpinBox(QWidget* parent = 0);

    long long value() const;

    QString prefix() const;
    void setPrefix(const QString& prefix);

    QString suffix() const;
    void setSuffix(const QString& suffix);

    QString cleanText() const;
    void setTotalLength(int length);
    long long singleStep() const;
    void setSingleStep(long long val);

    long long minimum() const;
    void setMinimum(long long min);

    long long maximum() const;
    void setMaximum(long long max);

    void setRange(long long min, long long max);

//    QString textFromValue(int value) const override;

public slots:
    void setValue(long long value);

signals:
    void valueChanged(long long i);
    void textChanged(const QString& text);

protected:
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void focusOutEvent(QFocusEvent* event);
    virtual void stepBy(int steps);
    virtual StepEnabled stepEnabled() const;
    virtual QValidator::State validate(QString& input, int& pos) const;

private:
    void lineEditEditingFinalize();
    void selectCleanText();

private:
    QString m_prefix;
    QString m_suffix;
    long long m_singleStep;
    long long m_minimum;
    long long m_maximum;
    long long m_value;

    int m_totalLength{0};

private:
    Q_DISABLE_COPY(LeadingZeroSpinBox)
};

