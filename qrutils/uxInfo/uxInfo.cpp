#include <QtCore/QDir>
#include <QtCore/QPointF>
#include <QtGui/QScreen>
#include <QApplication>

#include "uxInfo.h"

using namespace utils;

UXInfo* UXInfo::object = NULL;

QString const elementCreationFileName = "/elementOnSceneCreation.txt";
QString const errorReporterFileName = "/errorReporter.txt";
QString const totalTimeFileName = "/totalTime.txt";
QString const menuElementUsingFileName = "/menuElementUsing.txt";
QString const mouseClickPositionFileName = "/mouseClickPosition.txt";
QString const settingChangesFileName = "/settingChanges.txt";
QString const userActionFileName = "/userActions.txt";
QString const uxInfoDirName = "usabilityFiles";

UXInfo::UXInfo()
{
	QDir dir(".");

	///create and open all files
	mElementOnSceneCreationFile.setFileName(dir.absolutePath() + elementCreationFileName);
	if (mElementOnSceneCreationFile.open(QFile::WriteOnly | QFile::Truncate)) {
		mElementOnSceneCreationStream.setDevice(&mElementOnSceneCreationFile);
	}

	mCreationNumber = 1;

	mErrorReporterFile.setFileName(dir.absolutePath() + errorReporterFileName);
	if (mErrorReporterFile.open(QFile::WriteOnly | QFile::Truncate)) {
		mErrorReporterStream.setDevice(&mErrorReporterFile);
	}

	mErrorReporterNumber = 1;

	mTotalTimeFile.setFileName(dir.absolutePath() + totalTimeFileName);
	if (mTotalTimeFile.open(QFile::WriteOnly | QFile::Truncate)) {
		mTotalTimeStream.setDevice(&mTotalTimeFile);
	}

	mMenuElementUsingFile.setFileName(dir.absolutePath() + menuElementUsingFileName);
	if (mMenuElementUsingFile.open(QFile::WriteOnly | QFile::Truncate)) {
		mMenuElementUsingStream.setDevice(&mMenuElementUsingFile);
	}

	mMenuElementUsingNumber = 1;

	mMouseClickPositionFile.setFileName(dir.absolutePath() + mouseClickPositionFileName);
	if (mMouseClickPositionFile.open(QFile::WriteOnly | QFile::Truncate)) {
		mMouseClickPositionStream.setDevice(&mMouseClickPositionFile);
	}

	mMouseClickPositionNumber = 1;

	mSettingChangesFile.setFileName(dir.absolutePath() + settingChangesFileName);
	if (mSettingChangesFile.open(QFile::WriteOnly | QFile::Truncate)) {
		mSettingChangesStream.setDevice(&mSettingChangesFile);
	}

	mSettingChangesNumber = 1;

	mUserActionFile.setFileName(dir.absolutePath() + userActionFileName);
	if (mUserActionFile.open(QFile::WriteOnly | QFile::Truncate)) {
		mUserActionStream.setDevice(&mUserActionFile);
	}

	mUserActionNumber = 1;

	mTestNumber = 1;
	mNotEnoughDiskSpace = false;

	mLastX = 0;
	mLastY = 0;
	mLength = 0;
	mLastUserActionDateTime = QDateTime();

	QScreen *screen = QApplication::screens().at(0);
	mDpi = (qreal)screen->logicalDotsPerInch();
}

bool UXInfo::writeData(QTextStream const &stream)
{
	if (!mStatus || mNotEnoughDiskSpace) {
		return false;
	}

	if (stream.status() == QTextStream::WriteFailed) {
		mNotEnoughDiskSpace = true;
		return false;
	}
	return true;
}

void UXInfo::reportCreationOfElements(const QString &editorName, const QString elementName)
{
	if (!writeData(mElementOnSceneCreationStream)) {
		return;
	}

	mElementOnSceneCreationStream << mCreationNumber << " "
			<< editorName << " "
			<< elementName << " "
			<< currentDateTime() << "\n";

	mCreationNumber++;
}

void UXInfo::reportErrorsOfElements(const QString &type, const QString &editorName, const QString &elementName, const QString &message)
{
	if (!writeData(mErrorReporterStream)) {
		return;
	}

	mErrorReporterStream << mErrorReporterNumber << " "
			<< type << " "
			<< editorName << " "
			<< elementName << " "
			<< message << " "
			<< currentDateTime() << "\n";

	mErrorReporterNumber++;
}

void UXInfo::reportTotalTimeOfExec(const QString &totalTime, const int &exitCode)
{
	if (!writeData(mTotalTimeStream)) {
		return;
	}

	mTotalTimeStream << "TotalSessionTime: "
			<< totalTime << " secs Exit code:"
			<< exitCode << "\n";
}

void UXInfo::reportMenuElementsUsing(const QString &elementName, const QString &status)
{
	if (!writeData(mMenuElementUsingStream)) {
		return;
	}

	QString const statusText = (status == "none") ? "" : status + " ";
	mMenuElementUsingStream << mMenuElementUsingNumber << " "
			<< elementName << " "
			<< statusText
			<< currentDateTime() << "\n";

	mMenuElementUsingNumber++;
}

void UXInfo::reportMouseClickPosition(const QPoint &pos)
{
	if (!writeData(mMouseClickPositionStream)) {
		return;
	}

	int currentPosX = pos.x();
	int currentPosY = pos.y();
	int currentLength = static_cast<int>(sqrt(pow((currentPosX - mLastX), 2) + pow((currentPosY - mLastY), 2)));
	mLength += currentLength;
	mLastX = currentPosX;
	mLastY = currentPosY;
	mMouseClickPositionStream << mMouseClickPositionNumber << " ("
			//<< QString::number(pos.x()) << ", "
			//<< QString::number(pos.y()) << ") "
			<< QString::number(mLength) << ") "
			<< currentDateTime() << "\n";

	mMouseClickPositionNumber++;
}

void UXInfo::reportSettingsChangesInfo(const QString &name, const QString &oldValue, const QString &newValue)
{
	if (!writeData(mSettingChangesStream)) {
		return;
	}

	mSettingChangesStream << mSettingChangesNumber << " "
			<< name << " "
			<< oldValue << " "
			<< newValue << " "
			<< currentDateTime() << "\n";

	mSettingChangesNumber++;
}

void UXInfo::reportPaletteUserActionInfo(const QString &userAction)
{
	if (!writeData(mUserActionStream)) {
		return;
	}

	QString currentActionDateTime;
	QDateTime current = QDateTime::currentDateTime();
	if (mLastUserActionDateTime.isNull()) {
		currentActionDateTime = currentDateTime();
	}
	else {
		currentActionDateTime = "+" + QString::number(mLastUserActionDateTime.msecsTo(QDateTime::currentDateTime())) + " msec";
	}
	mLastUserActionDateTime = current;

	mUserActionStream << mUserActionNumber << " "
			<< userAction << " "
			<< currentActionDateTime << "\n";

	mUserActionNumber++;
}

void UXInfo::setActualStatus(bool status)
{
	mStatus = status;
}

void UXInfo::reportTestStartedInfo()
{
	if (!writeData(mSettingChangesStream)) {
		return;
	}

	QList<QTextStream *> streamList;
	streamList << &mElementOnSceneCreationStream << &mErrorReporterStream << &mTotalTimeStream
			<< &mMenuElementUsingStream << &mMouseClickPositionStream << &mSettingChangesStream;

	QString const now = currentDateTime();
	for (int i = 0; i < streamList.length(); ++i) {
		*(streamList[i]) << "Test " << mTestNumber << " started at " << now << "\n";
	}
}

void UXInfo::reportTestFinishedInfo()
{
	if (!writeData(mSettingChangesStream)) {
		return;
	}

	QList<QTextStream *> streamList;
	streamList << &mElementOnSceneCreationStream << &mErrorReporterStream << &mTotalTimeStream
			<< &mMenuElementUsingStream << &mMouseClickPositionStream << &mSettingChangesStream;

	QString const now = currentDateTime();
	for (int i = 0; i < streamList.length(); ++i) {
		*(streamList[i]) << "Test " << mTestNumber << " finished at " << now << "\n";
	}
	mTestNumber++;
}

void UXInfo::closeUXFiles()
{
	mElementOnSceneCreationFile.close();
	mErrorReporterFile.close();
	mTotalTimeFile.close();
	mMenuElementUsingFile.close();
	mMouseClickPositionFile.close();
	mSettingChangesFile.close();
	mUserActionFile.close();
}

QString UXInfo::currentDateTime()
{
	return QDateTime::currentDateTime().toString("yyyy.MM.dd_hh.mm.ss.zzz");
}

UXInfo *UXInfo::instance()
{
	if (object == NULL) {
		object = new UXInfo();
	}

	return object;
}

void UXInfo::closeUXInfo()
{
	closeUXFiles();

	if (!mStatus) {
		return;
	}

	QDir dir(".");
	QString const newDirName = currentDateTime();
	QString const oldElementOnSceneCreationName = mElementOnSceneCreationFile.fileName();
	QString const oldErrorReporterName = mErrorReporterFile.fileName();
	QString const oldTotalTimeName = mTotalTimeFile.fileName();
	QString const oldMenuElementUsingName = mMenuElementUsingFile.fileName();
	QString const oldMouseClickPositionName = mMouseClickPositionFile.fileName();
	QString const oldSettingChangesName = mSettingChangesFile.fileName();
	QString const oldUserActionName = mUserActionFile.fileName();

	QString const newFileElementOnSceneCreationName = newDirName + elementCreationFileName;
	QString const newFileErrorReporterName = newDirName + errorReporterFileName;
	QString const newFileTotalTimeName = newDirName + totalTimeFileName;
	QString const newFileMenuElementUsingName = newDirName + menuElementUsingFileName;
	QString const newFileMouseClickPositionName = newDirName + mouseClickPositionFileName;
	QString const newFileSettingChangesName = newDirName + settingChangesFileName;
	QString const newFileUserActionName = newDirName + userActionFileName;

	if (dir.cdUp()) {
		if (!dir.exists(uxInfoDirName)) {
			dir.mkdir(uxInfoDirName);
		}

		dir.cd(uxInfoDirName);
		QString const dirAbsolutePathName = dir.absolutePath() + "/";
		QString const newElementOnSceneCreationName = dirAbsolutePathName + newFileElementOnSceneCreationName;
		QString const newErrorReporterName = dirAbsolutePathName + newFileErrorReporterName;
		QString const newTotalTimeName = dirAbsolutePathName + newFileTotalTimeName;
		QString const newMenuElementUsingName = dirAbsolutePathName + newFileMenuElementUsingName;
		QString const newMouseClickPositionName = dirAbsolutePathName + newFileMouseClickPositionName;
		QString const newSettingChangesName = dirAbsolutePathName + newFileSettingChangesName;
		QString const newUserActionName = dirAbsolutePathName + newFileUserActionName;

		dir.mkdir(newDirName);
		QFile::copy(oldElementOnSceneCreationName, newElementOnSceneCreationName);
		QFile::copy(oldErrorReporterName, newErrorReporterName);
		QFile::copy(oldTotalTimeName, newTotalTimeName);
		QFile::copy(oldMenuElementUsingName, newMenuElementUsingName);
		QFile::copy(oldMouseClickPositionName, newMouseClickPositionName);
		QFile::copy(oldSettingChangesName, newSettingChangesName);
		QFile::copy(oldUserActionName, newUserActionName);
	}
}

void UXInfo::reportCreation(const QString &editorName, const QString elementName)
{
	instance()->reportCreationOfElements(editorName, elementName);
}

void UXInfo::reportErrors(const QString &type, const QString &editorName, const QString &elementName,
		const QString &message)
{
	instance()->reportErrorsOfElements(type, editorName, elementName, message);
}

void UXInfo::reportTotalTime(QString const &totalTime, int const &exitCode)
{
	instance()->reportTotalTimeOfExec(totalTime, exitCode);
}

void UXInfo::reportMenuElements(const QString &elementName, const QString &status)
{
	instance()->reportMenuElementsUsing(elementName, status);
}

void UXInfo::reportMouseClick(const QPoint &pos)
{
	instance()->reportMouseClickPosition(pos);
}

void UXInfo::reportSettingsChanges(const QString &name, const QVariant &oldValue, const QVariant &newValue)
{
	instance()->reportSettingsChangesInfo(name, oldValue.toString(), newValue.toString());
}

void UXInfo::reportPaletteUserAction(const QString &userAction)
{
	instance()->reportPaletteUserActionInfo(userAction);
}

void UXInfo::setStatus(bool status)
{
	instance()->setActualStatus(status);
}

void UXInfo::reportTestStarted()
{
	instance()->reportTestStartedInfo();
}

void UXInfo::reportTestFinished()
{
	instance()->reportTestFinishedInfo();
}

UXInfo::~UXInfo()
{
}
