#include "generatorBase/semanticTree/loopNode.h"

#include <qrutils/stringUtils.h>

using namespace generatorBase::semantics;
using namespace qReal;

LoopNode::LoopNode(const Id &idBinded, QObject *parent)
	: ConditionalNode(idBinded, parent)
	, mBodyZone(new ZoneNode(this))
	, mDoWhileForm(false)
{
	mBodyZone->setParentNode(this);
}

QString LoopNode::toStringImpl(GeneratorCustomizer &customizer, int indent, const QString &indentString) const
{
	simple::AbstractSimpleGenerator *generator = nullptr;
	if (mId.isNull()) {
		// Then this loop is generated by arrow back from simple block
		generator = customizer.factory()->infiniteLoopGenerator(mId, customizer);
	} else {
		if (customizer.semanticsOf(mId) == enums::semantics::loopBlock) {
			generator = customizer.factory()->forLoopGenerator(mId, customizer);
			// Then this loop is generated by a regular loop block usage
		} else {
			// Then this loop is generated by arrow back from if block
			generator = customizer.factory()->whileLoopGenerator(mId, customizer
					, mDoWhileForm, mAddNotToCondition);
		}
	}

	QString result = utils::StringUtils::addIndent(generator->generate(), indent, indentString);
	const QString bodyCode = mBodyZone->toString(customizer, indent + 1, indentString);
	result.replace("@@BODY@@", bodyCode);
	return result;
}

void LoopNode::appendChildren(QLinkedList<SemanticNode *> const &nodes)
{
	mBodyZone->appendChildren(nodes);
}

void LoopNode::setForm(bool doWhileForm)
{
	mDoWhileForm = doWhileForm;
}

ZoneNode *LoopNode::bodyZone()
{
	return mBodyZone;
}

QLinkedList<SemanticNode *> LoopNode::children() const
{
	return QLinkedList<SemanticNode *>() << mBodyZone;
}
