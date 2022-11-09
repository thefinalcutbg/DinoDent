#include "Theme.h"

QPainterPath Theme::getHalfCurvedPath(int width, int height)
{
	QPainterPath path;

	path.moveTo(radius, 0);
	path.lineTo(width, 0);
	path.lineTo(width, height);
	path.lineTo(radius, height);
	path.arcTo(0, height - radius, radius, radius, -90, -90);
	path.lineTo(0, radius);
	path.arcTo(0, 0, radius, radius, 180, -90);
	path.lineTo(radius, 0);

	return path;
}

QPainterPath Theme::getOneCornerCurvedRect(int width, int height)
{
	QPainterPath path;

	path.moveTo(radius, 0);
	path.lineTo(width, 0);
	path.lineTo(width, height);
	path.lineTo(0, height);
	path.lineTo(0, radius);
	path.arcTo(0, 0, radius, radius, 180, -90);
	path.lineTo(radius, 0);

	return path;
}

QString Theme::colorToString(const QColor& color)
{
	QString style = " rgb(%1, %2, %3);";
	return QString(style.arg(color.red()).arg(color.green()).arg(color.blue()));
}

QString Theme::getFancyStylesheet()
{
	return QString(
			"QLabel{ color :" + Theme::colorToString(Theme::fontTurquoise) + ";}"
			
			"QCheckBox{"
				"color :" + Theme::colorToString(Theme::fontTurquoise) + ";"
			"}"

			"QRadioButton{"
				"color :" + Theme::colorToString(Theme::fontTurquoise) + ";"
			"}"

			"QTableView{"
			"color :" + Theme::colorToString(Theme::fontTurquoise) + "; "
			"selection-color:" + Theme::colorToString(Theme::fontTurquoiseClicked) + "; "
			"selection-background-color: " + Theme::colorToString(Theme::background) + "; "
			"}"

			"QComboBox{"
			"color: " + Theme::colorToString(Theme::fontTurquoise) + "; "
			"background-color:" + Theme::colorToString(Theme::sectionBackground) + "; "
			"border: 1px solid; "
			"border-color: " + Theme::colorToString(Theme::buttonFrame) + "; "
			"border-radius: 10px; "
			"padding: 3px 0px 3px 10px;"
			"}"

			"QAbstractSpinBox{"
			"color: " + Theme::colorToString(Theme::fontTurquoise) + "; "
			"background-color:" + Theme::colorToString(Theme::sectionBackground) + "; "
			"border: 1px solid; "
			"border-color: " + Theme::colorToString(Theme::buttonFrame) + "; "
			"border-radius: 10px; "
			"padding: 3px 0px 3px 10px;"
			"}"

			"QComboBox::hover{"
			"border-color: " + Theme::colorToString(Theme::fontTurquoiseClicked) + "; "
			"color: " + Theme::colorToString(Theme::fontTurquoise) + "; "
			"background-color:" + Theme::colorToString(Theme::background) + ";"
			"}"

			"QComboBox::drop-down:button{"
			"background-color: transparent; "
			"}"

			"QComboBox::down-arrow {"
			"image: url(:/icons/icon_downArrow.png);"
			"width: 10px;"
			"height: 10px;"
			"}"

			"QComboBox::down-arrow::disabled {"
			"image: none"
			"}"

			"QDateEdit{"
			"color: " + Theme::colorToString(Theme::fontTurquoise) + "; "
			"background-color:" + Theme::colorToString(Theme::sectionBackground) + "; "
			"border: 1px solid; "
			"border-color: " + Theme::colorToString(Theme::buttonFrame) + "; "
			"border-radius: 10px; "
			"padding: 3px 0px 3px 10px;"
			"selection-background-color:" + Theme::colorToString(Theme::background) + ";"
			"selection-color:" + Theme::colorToString(Theme::fontTurquoiseClicked) + ";"
			"}"

			"QDateEdit::hover{"
			"border-color: " + Theme::colorToString(Theme::fontTurquoiseClicked) + "; "
			"color: " + Theme::colorToString(Theme::fontTurquoise) + "; "
			"background-color:" + Theme::colorToString(Theme::background) + ";"
			"}"

			"QDateEdit::drop-down:button{"
			"background-color: transparent; "
			"}"

			"QDateEdit::down-arrow {"
			"image: url(:/icons/icon_downArrow.png);"
			"width: 10px;"
			"height: 10px;"
			"}"

			"QDateEdit::down-arrow::disabled {"
			"image: none"
			"}"
	);
}

QString Theme::getPopupMenuStylesheet()
{
	return
		"QMenu{background-color:white}"
		"QMenu::item::selected{background-color:rgb(144,200,246); color:black}"
		;
}