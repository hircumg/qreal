#pragma once

#include <QtCore/QMap>
#include <QtWidgets/QWidget>

#include <interpreterBase/sensorsConfigurationProvider.h>
#include <interpreterBase/robotModel/portInfo.h>
#include <interpreterBase/robotModel/pluggableDeviceInfo.h>

class QComboBox;
class QVBoxLayout;

namespace interpreterBase {
namespace robotModel {
class RobotModelInterface;
}
}

namespace interpreterCore {
namespace ui {

/// A number of combo boxes for sensors selection
class SensorsConfigurationWidget : public QWidget, public interpreterBase::SensorsConfigurationProvider
{
	Q_OBJECT

public:
	/// @param autosaveMode Should save happed on each modification or would
	/// be called manually
	explicit SensorsConfigurationWidget(QWidget *parent = 0, bool autosaveMode = false);

	void loadRobotModels(QList<interpreterBase::robotModel::RobotModelInterface *> const &models);

	/// Reinits current widget for the given robot model
	void selectRobotModel(interpreterBase::robotModel::RobotModelInterface &robotModel);

public slots:
	void refresh();
	void save();

private:
	QWidget *configurerForRobotModel(interpreterBase::robotModel::RobotModelInterface &robotModel);

	QLayout *initPort(QString const &robotModel
			, interpreterBase::robotModel::PortInfo const &port
			, QList<interpreterBase::robotModel::PluggableDeviceInfo> const &sensors);

	void hideAllConfigurers();

	void onSensorConfigurationChanged(QString const &robotModel
			, interpreterBase::robotModel::PortInfo const &port
			, interpreterBase::robotModel::PluggableDeviceInfo const &sensor);

	void propagateChanges(interpreterBase::robotModel::PortInfo const &port
			, interpreterBase::robotModel::PluggableDeviceInfo const &sensor);

	bool areConvertible(interpreterBase::robotModel::PortInfo const &port1
			, interpreterBase::robotModel::PortInfo const &port2) const;
	interpreterBase::robotModel::PluggableDeviceInfo convertibleDevice(
			interpreterBase::robotModel::RobotModelInterface const *robotModel
			, interpreterBase::robotModel::PortInfo const &port
			, interpreterBase::robotModel::PluggableDeviceInfo const &device) const;

	bool mAutosaveMode;
	QVBoxLayout *mLayout;
	QString mCurrentModel;
	QMap<QString, interpreterBase::robotModel::RobotModelInterface *> mRobotModels;  // Does not take ownership
	QMap<QString, QWidget *> mRobotModelConfigurers;  // Does not take ownership
	QList<QComboBox *> mConfigurers;
	bool mSaving;
	bool mRefreshing;
};

}
}
