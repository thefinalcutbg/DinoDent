#include "Theme.h"

#include <QStyleFactory>
#include <QStyle>
#include <QGraphicsDropShadowEffect>
#include <QWidget>
#include "src/Version.h"

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

			"QToolTip { "
			"background-color:" + Theme::colorToString(Theme::sectionBackground) + "; "
			"}"

            "QLineEdit{ "
            "color :" + Theme::colorToString(Theme::fontTurquoise) + ";"
            "border: 1px solid; "
            "border-color: " + Theme::colorToString(Theme::border) + "; "
            "border-radius: 10px; "
            "padding: 3px 0px 3px 3px;"
            "}"

			"QCheckBox{"
				"color :" + Theme::colorToString(Theme::fontTurquoise) + ";"
			"}"

			"QRadioButton{"
				"color :" + Theme::colorToString(Theme::fontTurquoise) + ";"
			"}"

			"QTableView{"
            "color :" + Theme::colorToString(Theme::fontTurquoise) + "; "
            "selection-color:" + Theme::colorToString(Theme::fontTurquoise) + "; "
            "selection-background-color: " + Theme::colorToString(Theme::background) + "; "
            "background: transparent;"
			"}"


            "QTableView::viewport { background: transparent; }"
            "QHeaderView { background: transparent; }"
            "QHeaderView::section { background: transparent; }"
            "QTableCornerButton::section { background: transparent; }"

			"QComboBox{"
			"color: " + Theme::colorToString(Theme::fontTurquoise) + "; "
			"background-color:" + Theme::colorToString(Theme::sectionBackground) + "; "
			"border: 1px solid; "
			"border-color: " + Theme::colorToString(Theme::buttonFrame) + "; "
			"border-radius: 10px; "
			"padding: 3px 0px 3px 10px;"
			"}"

			"QGroupBox:title{ color:" + Theme::colorToString(Theme::fontTurquoise) + "; }"

			"QAbstractSpinBox{"
			"color: " + Theme::colorToString(Theme::fontTurquoise) + "; "
			"background-color:" + Theme::colorToString(Theme::sectionBackground) + "; "
			"border: 1px solid; "
			"border-color: " + Theme::colorToString(Theme::buttonFrame) + "; "
			"border-radius: 10px; "
			"padding: 3px 0px 3px 3px;"
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

			"QDateTimeEdit::down-arrow {"
			"image: url(:/icons/icon_downArrow.png);"
			"width: 10px;"
			"height: 10px;"
			"}"

			"QDateTimeEdit::drop-down:button{"
			"background-color: transparent; "
			"}"


			"QDateTimeEdit::down-arrow::disabled {"
			"image: none"
			"}"

            "QTreeWidget{"
            "color: " + Theme::colorToString(Theme::fontTurquoise) + "; "
            "selection-background-color: " + Theme::colorToString(Theme::background) + "; "
            "selection-color: " + Theme::colorToString(Theme::fontTurquoise) + "; "
            "}"
            "QTreeWidget::item{"
            "padding: 4px 6px;"
            "}"

            "QTreeWidget::item:selected{"
            "background-color: " + Theme::colorToString(Theme::background) + "; "
            "color: " + Theme::colorToString(Theme::fontTurquoise) + "; "
            "}"

            "QHeaderView::section{"
            "color: " + Theme::colorToString(Theme::fontTurquoise) + "; "
            "background-color: " + Theme::colorToString(Theme::sectionBackground) + "; "
            "border: 0px; "
            "padding: 6px 8px;"
            "}"

            "QTreeView::branch:closed:has-children{"
            "image: url(:/icons/icon_rightArrow.png);"
            "}"
            "QTreeView::branch:open:has-children{"
            "image: url(:/icons/icon_downArrow.png);"
            "}"
	);
}

QString Theme::getPopupMenuStylesheet()
{
	return
		"QMenu{background-color:white; border:1px solid #ccc; "
#ifdef Q_OS_OSX
		"border-radius:10px; "
#endif
		"padding:8px 6px;}"
		"QMenu::icon{ padding-left:6px; padding-right:8px; }"
		"QMenu::item{ padding:6px 28px 6px 6px; margin:2px 0px; border-radius:6px; }"
		"QMenu::item:selected{ background-color:" + colorToString(Theme::buttonFrame) + "; color:black; }"
		"QMenu::right-arrow{ image:url(:/icons/nextMonth.png); width:15px; height:15px; }"
		;
}

QStyle* s_fusion = nullptr;

QStyle* Theme::fusionStyle()
{
    if(!s_fusion){
        s_fusion = QStyleFactory::create("Fusion");
    }

    return s_fusion;
}

void Theme::cleanUpFusionStyle()
{
    if(s_fusion) {
        delete s_fusion;
    }

}

void Theme::applyShadow(QWidget *w, int blur, QPoint offset, QColor color)
{
    auto* fx = new QGraphicsDropShadowEffect(w);
    fx->setBlurRadius(blur);
    fx->setOffset(offset);
    fx->setColor(color);
    w->setGraphicsEffect(fx);
}

void Theme::applyLightShadow(QWidget *w)
{
    Theme::applyShadow(w, 14, {0, 1}, QColor(0, 0, 0, 22));
}
