#include "setBackgroundBlock.h"

#include <QtGui/QColor>

#include <interpreterBase/robotModel/robotModelUtils.h>

#include "src/robotModel/parts/trikDisplay.h"

using namespace trikKitInterpreter::blocks::details;
using namespace interpreterBase::robotModel::robotParts;

SetBackgroundBlock::SetBackgroundBlock(interpreterBase::robotModel::RobotModelInterface &robotModel)
	: mRobotModel(robotModel)
{
}

void SetBackgroundBlock::run()
{
	robotModel::parts::TrikDisplay * const display =
			interpreterBase::robotModel::RobotModelUtils::findDevice<robotModel::parts::TrikDisplay>(
					mRobotModel, "DisplayPort"
			);

	display->setBackground(propertyToColor(stringProperty("Color")));
	emit done(mNextBlockId);
}

QColor SetBackgroundBlock::propertyToColor(QString const &property) const
{
	if (property == "white") {
		return QColor(Qt::white);
	} else if (property == "black") {
		return QColor(Qt::black);
	} else if (property == "red") {
		return QColor(Qt::red);
	} else if (property == "darkRed") {
		return QColor(Qt::darkRed);
	} else if (property == "green") {
		return QColor(Qt::green);
	} else if (property == "darkGreen") {
		return QColor(Qt::darkGreen);
	} else if (property == "blue") {
		return QColor(Qt::blue);
	} else if (property == "darkBlue") {
		return QColor(Qt::darkBlue);
	} else if (property == "cyan") {
		return QColor(Qt::cyan);
	} else if (property == "darkCyan") {
		return QColor(Qt::darkCyan);
	} else if (property == "magenta") {
		return QColor(Qt::magenta);
	} else if (property == "darkMagenta") {
		return QColor(Qt::darkMagenta);
	} else if (property == "yellow") {
		return QColor(Qt::yellow);
	} else if (property == "darkYellow") {
		return QColor(Qt::darkYellow);
	} else if (property == "gray") {
		return QColor(Qt::gray);
	} else if (property == "darkGray") {
		return QColor(Qt::darkGray);
	} else if (property == "lightGray") {
		return QColor(Qt::lightGray);
	}

	return QColor(Qt::transparent);
}
