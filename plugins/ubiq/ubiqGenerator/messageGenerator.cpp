#include "messageGenerator.h"

#include "nameNormalizer.h"

#include <QtCore/QFile>

#include <QtCore/QDebug>

using namespace ubiq::generator;
using namespace qReal;

QString const fileName = "Message.cs";

MessageGenerator::MessageGenerator(QString const &templateDirPath
		, QString const &outputDirPath
		, qReal::LogicalModelAssistInterface const &logicalModel
		, qReal::ErrorReporterInterface &errorReporter
		)
		: AbstractGenerator(templateDirPath, outputDirPath, logicalModel, errorReporter)
{
}

MessageGenerator::~MessageGenerator()
{
}

void MessageGenerator::generate()
{
	QString result;
	loadTemplateFromFile(fileName, result);
	loadTemplateUtils();

	QDir dir;
	if (!dir.exists(mOutputDirPath))
		dir.mkdir(mOutputDirPath);
	dir.cd(mOutputDirPath);

	foreach (Id const diagram, mApi.elementsByType("DataStructuresDiagram")) {
		if (!mApi.isLogicalElement(diagram))
			continue;

		foreach (Id const element, mApi.children(diagram)) {
			if (!mApi.isLogicalElement(element))
				continue;

			if (element.element() == "MessageClass") {
				result.replace("@@Properties@@", generatePropertiesCode(element))
						.replace("@@InitFieldsWithDefaults@@", generateDefaultFieldsInitialization(element))
						.replace("@@ConstructorArgs@@", generateConstructorArguments(element))
						.replace("@@InitFieldsWithArgs@@", generateFieldsInitialization(element))
						.replace("@@ConstructorActualArgs@@", generateConstructorActualArguments(element))
						.replace("@@PackFields@@", generatePackFields(element))
						.replace("@@UnpackFields@@", generateUnpackFields(element))
						;
			} else if (element.element() == "MessageCodes") {
				result.replace("@@MessageCodes@@", generateEnumElements(element));
			} else if (element.element() == "ErrorCodes") {
				result.replace("@@ErrorCodes@@", generateEnumElements(element));
			}
		}
	}

	saveOutputFile(dir.absoluteFilePath(fileName), result);
}

QString MessageGenerator::generateEnumElements(qReal::Id const &element)
{
	QString result;
	int value = 0;
	foreach (Id const id, mApi.children(element)) {
		if (!mApi.isLogicalElement(id) || id.element() != "EnumElement")
			continue;

		QString propertyTemplate = mTemplateUtils["@@EnumElement@@"];
		QString name = mApi.name(id);
		propertyTemplate.replace("@@Name@@", NameNormalizer::normalize(name))
				.replace("@@Value@@", QString::number(value));

		result += propertyTemplate;
		++value;
	}
	return result;
}

QString MessageGenerator::generateDefaultFieldsInitialization(qReal::Id const &element)
{
	QString fieldsInitialization;
	foreach (Id const property, mApi.children(element)) {
		if (!mApi.isLogicalElement(property) || property.element() != "Field")
			continue;

		QString initTemplate = mTemplateUtils["@@FieldInit@@"];
		QString const name = mApi.name(property);

		QString const defaultValue = mApi.stringProperty(property, "defaultValue").isEmpty()
				? getDefaultValue(mApi.stringProperty(property, "type"))
				: mApi.stringProperty(property, "defaultValue");

		initTemplate.replace("@@Name@@", NameNormalizer::normalize(name, false))
				.replace("@@Value@@", defaultValue)
				;

		fieldsInitialization += ("    " + initTemplate);
	}
	return fieldsInitialization;
}

QString MessageGenerator::generateFieldsInitialization(qReal::Id const &element)
{
	QString fieldsInitialization;
	foreach (Id const property, mApi.children(element)) {
		if (!mApi.isLogicalElement(property) || property.element() != "Field")
			continue;

		QString initTemplate = mTemplateUtils["@@FieldInit@@"];
		QString argName = NameNormalizer::normalize(mApi.name(property), false);
		initTemplate.replace("@@Name@@", argName)
				.replace("@@Value@@", argName)
				;

		fieldsInitialization += ("    " + initTemplate);
	}
	return fieldsInitialization;
}

QString MessageGenerator::generateConstructorArguments(qReal::Id const &element)
{
	QString parametersList;
	foreach (Id const property, mApi.children(element)) {
		if (!mApi.isLogicalElement(property) || property.element() != "Field")
			continue;

		QString argumentTemplate = mTemplateUtils["@@Argument@@"];
		QString argName = NameNormalizer::normalize(mApi.name(property), false);
		QString type = mApi.stringProperty(property, "type");
		argumentTemplate.replace("@@ArgType@@", type).replace("@@ArgName@@", argName);
		parametersList += (argumentTemplate + ", ");
	}
	return parametersList;
}

QString MessageGenerator::generateConstructorActualArguments(qReal::Id const &element)
{
	QString parametersList;
	foreach (Id const property, mApi.children(element)) {
		if (!mApi.isLogicalElement(property) || property.element() != "Field")
			continue;

		QString argName = NameNormalizer::normalize(mApi.name(property), false);
		parametersList += (argName + ", ");
	}
	return parametersList;
}

QString MessageGenerator::generatePackFields(qReal::Id const &element)
{
	QString serializersList;
	foreach (Id const property, mApi.children(element)) {
		if (!mApi.isLogicalElement(property) || property.element() != "Field")
			continue;

		QString const name = NameNormalizer::normalize(mApi.name(property), false);
		QString const type = mApi.stringProperty(property, "type");

		QString serializationTemplate;

		if (type == "int") {
			bool const serializeAsShort = mApi.property(property, "serializeAsShort").toBool();
			if (serializeAsShort) {
				serializationTemplate = mTemplateUtils["@@SerializationIntAsShort@@"];
			}
		}
		if (serializationTemplate.isEmpty())
			serializationTemplate = mTemplateUtils["@@Serialization_" + type + "@@"];

		serializationTemplate.replace("@@Name@@", name);
		serializersList += serializationTemplate;
	}
	return serializersList;
}

QString MessageGenerator::generateUnpackFields(qReal::Id const &element)
{
	QString deserializersList;
	foreach (Id const property, mApi.children(element)) {
		if (!mApi.isLogicalElement(property) || property.element() != "Field")
			continue;

		QString const name = NameNormalizer::normalize(mApi.name(property), false);
		QString const type = mApi.stringProperty(property, "type");

		QString deserializationTemplate;

		if (type == "int") {
			bool const serializeAsShort = mApi.property(property, "serializeAsShort").toBool();
			if (serializeAsShort) {
				deserializationTemplate = mTemplateUtils["@@DeserializationIntAsShort@@"];
			}
		}
		if (deserializationTemplate.isEmpty())
			deserializationTemplate = mTemplateUtils["@@Deserialization_" + type + "@@"];

		deserializationTemplate.replace("@@Name@@", name);
		deserializersList += deserializationTemplate;
	}
	return deserializersList;
}
