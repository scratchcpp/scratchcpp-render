// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/costume.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/textbubble.h>

#include "stagemodel.h"
#include "penlayer.h"

using namespace scratchcpprender;

StageModel::StageModel(QObject *parent) :
    TargetModel(parent)
{
}

void StageModel::init(libscratchcpp::Stage *stage)
{
    m_stage = stage;

    if (m_stage)
        setupTextBubble(m_stage->bubble());
}

void StageModel::onCostumeChanged(libscratchcpp::Costume *costume)
{
    updateCostume(costume);
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
    setGraphicEffect(effect, value);
}

void StageModel::onGraphicsEffectsCleared()
{
    clearGraphicEffects();
}

int StageModel::costumeWidth() const
{
    return TargetModel::costumeWidth();
}

int StageModel::costumeHeight() const
{
    return TargetModel::costumeHeight();
}

libscratchcpp::Rect StageModel::boundingRect() const
{
    libscratchcpp::Rect ret;
    getBoundingRect(ret);
    return ret;
}

libscratchcpp::Rect StageModel::fastBoundingRect() const
{
    libscratchcpp::Rect ret;
    getFastBoundingRect(ret);
    return ret;
}

bool StageModel::touchingClones(const std::vector<libscratchcpp::Sprite *> &clones) const
{
    return TargetModel::touchingClones(clones);
}

bool StageModel::touchingPoint(double x, double y) const
{
    return TargetModel::touchingPoint(x, y);
}

bool StageModel::touchingColor(libscratchcpp::Rgb color) const
{
    return TargetModel::touchingColor(color);
}

bool StageModel::touchingColor(libscratchcpp::Rgb color, libscratchcpp::Rgb mask) const
{
    return TargetModel::touchingColor(color, mask);
}

libscratchcpp::Stage *StageModel::stage() const
{
    return m_stage;
}

int StageModel::bubbleLayer() const
{
    return m_stage ? m_stage->bubble()->layerOrder() : 0;
}

void StageModel::loadCostume()
{
    if (m_stage)
        updateCostume(m_stage->currentCostume().get());
}

void StageModel::drawPenPoint(IPenLayer *penLayer, const PenAttributes &penAttributes)
{
    penLayer->drawLine(penAttributes, 0, 0, 0, 0);
    libscratchcpp::IEngine *engine = m_stage->engine();

    if (engine)
        engine->requestRedraw();
}
