#pragma once

#include <interpreterBase/kitPluginInterface.h>
#include "nullRobotModel.h"

namespace nullKitInterpreter {

class NullKitInterpreterPlugin : public QObject, public interpreterBase::KitPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(interpreterBase::KitPluginInterface)
	Q_PLUGIN_METADATA(IID "nullKitInterpreter.NullKitInterpreterPlugin")

public:
	NullKitInterpreterPlugin();

	QString kitId() const override;

	QString friendlyKitName() const override;

	QList<interpreterBase::robotModel::RobotModelInterface *> robotModels() override;

	interpreterBase::blocksBase::BlocksFactoryInterface *blocksFactoryFor(
			const interpreterBase::robotModel::RobotModelInterface *model) override;

	// Transfers ownership.
	QList<interpreterBase::AdditionalPreferences *> settingsWidgets() override;

	QList<qReal::ActionInfo> customActions() override;

	QList<qReal::HotKeyActionInfo> hotKeyActions() override;

	QIcon iconForFastSelector(const interpreterBase::robotModel::RobotModelInterface &robotModel) const override;

private:
	NullRobotModel mRobotModel;
};

}
