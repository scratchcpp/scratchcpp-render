// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/costume.h>

#include "stagemodel.h"
#include "renderedtarget.h"

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
}

void StageModel::onGraphicsEffectsCleared()
{
}

void StageModel::loadCostume()
{
    if (m_renderedTarget && m_stage) {
        if (m_stage)
            m_renderedTarget->updateCostume(m_stage->currentCostume().get());
    }
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
    loadCostume();

    emit renderedTargetChanged();
}
