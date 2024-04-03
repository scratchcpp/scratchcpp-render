// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/costume.h>

#include "stagemodel.h"
#include "renderedtarget.h"
#include "graphicseffect.h"

using namespace scratchcpprender;

StageModel::StageModel(QObject *parent) :
    QObject(parent)
{
}

void StageModel::init(libscratchcpp::Stage *stage)
{
    m_stage = stage;
}

void StageModel::onCostumeChanged(libscratchcpp::Costume *costume)
{
    if (m_renderedTarget)
        m_renderedTarget->updateCostume(costume);
}

void StageModel::onTempoChanged(int tempo)
{
}

void StageModel::onVideoStateChanged(libscratchcpp::Stage::VideoState videoState)
{
}

void StageModel::onVideoTransparencyChanged(int videoTransparency)
{
}

void StageModel::onGraphicsEffectChanged(libscratchcpp::IGraphicsEffect *effect, double value)
{
    GraphicsEffect *graphicsEffect = dynamic_cast<GraphicsEffect *>(effect);

    if (graphicsEffect && m_renderedTarget)
        m_renderedTarget->setGraphicEffect(graphicsEffect->effect(), value);
}

void StageModel::onGraphicsEffectsCleared()
{
    if (m_renderedTarget)
        m_renderedTarget->clearGraphicEffects();
}

void StageModel::onBubbleTypeChanged(libscratchcpp::Target::BubbleType type)
{
    if (type == libscratchcpp::Target::BubbleType::Say) {
        if (m_bubbleType == TextBubbleShape::Type::Say)
            return;

        m_bubbleType = TextBubbleShape::Type::Say;
    } else {
        if (m_bubbleType == TextBubbleShape::Type::Think)
            return;

        m_bubbleType = TextBubbleShape::Type::Think;
    }

    emit bubbleTypeChanged();
}

void StageModel::onBubbleTextChanged(const std::string &text)
{
    QString newText = QString::fromStdString(text);

    if (m_bubbleText != newText) {
        m_bubbleText = newText;
        emit bubbleTextChanged();
    }
}

libscratchcpp::Rect StageModel::boundingRect() const
{
    return libscratchcpp::Rect();
}

libscratchcpp::Rect StageModel::fastBoundingRect() const
{
    return libscratchcpp::Rect();
}

bool StageModel::touchingClones(const std::vector<libscratchcpp::Sprite *> &clones) const
{
    return m_renderedTarget->touchingClones(clones);
}

bool StageModel::touchingPoint(double x, double y) const
{
    return m_renderedTarget->containsScratchPoint(x, y);
}

bool StageModel::touchingColor(const libscratchcpp::Value &color) const
{
    return false;
}

void StageModel::loadCostume()
{
    if (m_renderedTarget && m_stage)
        m_renderedTarget->updateCostume(m_stage->currentCostume().get());
}

libscratchcpp::Stage *StageModel::stage() const
{
    return m_stage;
}

IRenderedTarget *StageModel::renderedTarget() const
{
    return m_renderedTarget;
}

void StageModel::setRenderedTarget(IRenderedTarget *newRenderedTarget)
{
    if (m_renderedTarget == newRenderedTarget)
        return;

    m_renderedTarget = newRenderedTarget;

    emit renderedTargetChanged();
}

const TextBubbleShape::Type &StageModel::bubbleType() const
{
    return m_bubbleType;
}

const QString &StageModel::bubbleText() const
{
    return m_bubbleText;
}
