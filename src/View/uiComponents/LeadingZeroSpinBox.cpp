#include "LeadingZeroSpinBox.h"
#include "View/GlobalFunctions.h"

#include <QLineEdit>
#include <limits>
#include <QEvent>
#include <QKeyEvent>

LeadingZeroSpinBox::LeadingZeroSpinBox(QWidget* parent) :
    QAbstractSpinBox(parent)
{
    m_minimum = 1;
    m_maximum = std::numeric_limits<long long>::max();
    m_value = 1;
    m_singleStep = 1;

    setValue(m_value);
}

long long LeadingZeroSpinBox::value() const
{
    return m_value;
}

void LeadingZeroSpinBox::setValue(long long expectedNewValue)
{
    const long long newValue = qBound(m_minimum, expectedNewValue, m_maximum);

    QString formated;
    formated.reserve(m_totalLength);

    QString number = QString::number(expectedNewValue);

    for (int i = 0; i < m_totalLength - number.length(); i++) {
        formated += "0";
    }

    formated += number;

    lineEdit()->setText(m_prefix + formated + m_suffix);
    if (m_value != newValue) {
        m_value = newValue;

        emit valueChanged(newValue);
        emit textChanged(number);
    }
}

QString LeadingZeroSpinBox::prefix() const
{
    return m_prefix;
}

void LeadingZeroSpinBox::setPrefix(const QString& prefix)
{
    m_prefix = prefix;

    setValue(m_value);
}

QString LeadingZeroSpinBox::suffix() const
{
    return m_suffix;
}

void LeadingZeroSpinBox::setSuffix(const QString& suffix)
{
    m_suffix = suffix;

    setValue(m_value);
}

QString LeadingZeroSpinBox::cleanText() const
{
    return QString::number(m_value);
}

void LeadingZeroSpinBox::setTotalLength(int length)
{
    m_totalLength = length;
    lineEdit()->setMaxLength(m_totalLength);
    
    QString maximum;
    for (int i = 0; i < length; i++) {
        maximum.append('9');
    }

    setMaximum(maximum.toLongLong());

    setValue(m_value);

}

long long LeadingZeroSpinBox::singleStep() const
{
    return m_singleStep;
}

void LeadingZeroSpinBox::setSingleStep(long long step)
{
    m_singleStep = step;
}

long long LeadingZeroSpinBox::minimum() const
{
    return m_minimum;
}

void LeadingZeroSpinBox::setMinimum(long long min)
{
    m_minimum = min;
    if (m_maximum < m_minimum) {
        m_maximum = m_minimum;
    }

    setValue(m_value);
}

long long LeadingZeroSpinBox::maximum() const
{
    return m_maximum;
}

void LeadingZeroSpinBox::setMaximum(long long max)
{
    m_maximum = max;
    if (m_maximum < m_minimum) {
        m_maximum = m_minimum;
    }

    setValue(m_value);
}

void LeadingZeroSpinBox::setRange(long long min, long long max)
{
    if (min < max) {
        m_minimum = min;
        m_maximum = max;
    }
    else {
        m_minimum = max;
        m_maximum = min;
    }

    setValue(m_value);
}

void LeadingZeroSpinBox::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        selectCleanText();
        lineEditEditingFinalize();
    }

    QAbstractSpinBox::keyPressEvent(event);
}

void LeadingZeroSpinBox::focusOutEvent(QFocusEvent* event)
{
    lineEditEditingFinalize();

    QAbstractSpinBox::focusOutEvent(event);
}

QAbstractSpinBox::StepEnabled LeadingZeroSpinBox::stepEnabled() const
{
    if (isReadOnly()) {
        return QFlag(0);
    }

    StepEnabled se = QFlag(0);
    if (wrapping() || m_value < m_maximum) {
        se |= StepUpEnabled;
    }
    if (wrapping() || m_value > m_minimum) {
        se |= StepDownEnabled;
    }

    return se;
}

void LeadingZeroSpinBox::stepBy(int steps)
{
    if (isReadOnly()) {
        return;
    }

    if (m_prefix + QString::number(m_value) + m_suffix != lineEdit()->text()) {
        lineEditEditingFinalize();
    }

    long long newValue = m_value + (steps * m_singleStep);
    if (wrapping()) {
        // emulating the behavior of QSpinBox
        if (newValue > m_maximum) {
            if (m_value == m_maximum) {
                newValue = m_minimum;
            }
            else {
                newValue = m_maximum;
            }
        }
        else if (newValue < m_minimum) {
            if (m_value == m_minimum) {
                newValue = m_maximum;
            }
            else {
                newValue = m_minimum;
            }
        }
    }
    else {
        newValue = qBound(m_minimum, newValue, m_maximum);
    }

    setValue(newValue);
    selectCleanText();
}

QValidator::State LeadingZeroSpinBox::validate(QString& input, int& pos) const
{
    // first, we try to interpret as a number without prefixes
    bool ok;
    const long long value = input.toLongLong(&ok);
    if (input.isEmpty() || (ok && value <= m_maximum)) {
        input = m_prefix + input + m_suffix;
        pos += m_prefix.length();
        return QValidator::Acceptable;
    }

    // if string of text editor aren't simple number, try to interpret it
    // as a number with prefix and suffix
    bool valid = true;
    if (!m_prefix.isEmpty() && !input.startsWith(m_prefix)) {
        valid = false;
    }
    if (!m_suffix.isEmpty() && !input.endsWith(m_suffix)) {
        valid = false;
    }

    if (valid) {
        const int start = m_prefix.length();
        const int length = input.length() - start - m_suffix.length();

        bool ok;
        const QString number = input.mid(start, length);
        const long long value = number.toLongLong(&ok);
        if (number.isEmpty() || (ok && value <= m_maximum)) {
            return QValidator::Acceptable;
        }
    }

    // otherwise not acceptable
    return QValidator::Invalid;
}

void LeadingZeroSpinBox::lineEditEditingFinalize()
{
    const QString text = lineEdit()->text();

    // first, we try to read as a number without prefixes
    bool ok;
    long long value = text.toLongLong(&ok);
    if (ok) {
        setValue(value);
        return;
    }

    // if string of text editor aren't simple number, try to interpret it
    // as a number with prefix and suffix
    bool valid = true;
    if (!m_prefix.isEmpty() && !text.startsWith(m_prefix)) {
        valid = false;
    }
    else if (!m_suffix.isEmpty() && !text.endsWith(m_suffix)) {
        valid = false;
    }

    if (valid) {
        const int start = m_prefix.length();
        const int length = text.length() - start - m_suffix.length();

        bool ok;
        const long long value = text.mid(start, length).toLongLong(&ok);
        if (ok) {
            setValue(value);
            return;
        }
    }

    // otherwise set old value
    setValue(m_value);
}

void LeadingZeroSpinBox::selectCleanText()
{
    lineEdit()->setSelection(m_prefix.length(),
        lineEdit()->text().length()
        - m_prefix.length()
        - m_suffix.length());
}