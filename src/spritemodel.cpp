// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/costume.h>

#include "spritemodel.h"
#include "renderedtarget.h"

namespace scratchcppgui
{

SpriteModel::SpriteModel(QObject *parent) :
    QObject(parent)
{
}

void SpriteModel::init(libscratchcpp::Sprite *sprite)
{
    m_sprite = sprite;

    if (m_renderedTarget && sprite)
        m_renderedTarget->loadCostume(sprite->currentCostume().get());
}

void SpriteModel::onCloned(libscratchcpp::Sprite *clone)
{
}

void SpriteModel::onCostumeChanged(libscratchcpp::Costume *costume)
{
    if (m_renderedTarget)
        m_renderedTarget->loadCostume(costume);
}

void SpriteModel::onVisibleChanged(bool visible)
{
}

void SpriteModel::onXChanged(double x)
{
}

void SpriteModel::onYChanged(double y)
{
}

void SpriteModel::onSizeChanged(double size)
{
    if (m_renderedTarget)
        m_renderedTarget->loadCostume(m_sprite->currentCostume().get());
}

void SpriteModel::onDirectionChanged(double direction)
{
}

void SpriteModel::onRotationStyleChanged(libscratchcpp::Sprite::RotationStyle rotationStyle)
{
}

libscratchcpp::Sprite *SpriteModel::sprite() const
{
    return m_sprite;
}

IRenderedTarget *SpriteModel::renderedTarget() const
{
    return m_renderedTarget;
}

void SpriteModel::setRenderedTarget(IRenderedTarget *newRenderedTarget)
{
    if (m_renderedTarget == newRenderedTarget)
        return;

    m_renderedTarget = newRenderedTarget;

    if (m_renderedTarget && m_sprite)
        m_renderedTarget->loadCostume(m_sprite->currentCostume().get());

    emit renderedTargetChanged();
}

} // namespace scratchcppgui
