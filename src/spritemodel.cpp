// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/costume.h>
#include <scratchcpp/rect.h>

#include "spritemodel.h"
#include "renderedtarget.h"

namespace scratchcpprender
{

SpriteModel::SpriteModel(QObject *parent) :
    QObject(parent)
{
}

void SpriteModel::init(libscratchcpp::Sprite *sprite)
{
    m_sprite = sprite;
}

void SpriteModel::deinitClone()
{
    emit cloneDeleted(this);
}

void SpriteModel::onCloned(libscratchcpp::Sprite *clone)
{
    if (!m_cloneRoot)
        m_cloneRoot = this;

    SpriteModel *cloneModel = new SpriteModel(m_cloneRoot);
    cloneModel->m_cloneRoot = m_cloneRoot;
    clone->setInterface(cloneModel);
    emit cloned(cloneModel);
}

void SpriteModel::onCostumeChanged(libscratchcpp::Costume *costume)
{
    if (m_renderedTarget)
        m_renderedTarget->updateCostume(costume);
}

void SpriteModel::onVisibleChanged(bool visible)
{
    if (m_renderedTarget)
        m_renderedTarget->updateVisibility(visible);
}

void SpriteModel::onXChanged(double x)
{
    if (m_renderedTarget)
        m_renderedTarget->updateX(x);
}

void SpriteModel::onYChanged(double y)
{
    if (m_renderedTarget)
        m_renderedTarget->updateY(y);
}

void SpriteModel::onSizeChanged(double size)
{
    if (m_renderedTarget)
        m_renderedTarget->updateSize(size);
}

void SpriteModel::onDirectionChanged(double direction)
{
    if (m_renderedTarget)
        m_renderedTarget->updateDirection(direction);
}

void SpriteModel::onRotationStyleChanged(libscratchcpp::Sprite::RotationStyle rotationStyle)
{
    if (m_renderedTarget)
        m_renderedTarget->updateRotationStyle(rotationStyle);
}

void SpriteModel::onLayerOrderChanged(int layerOrder)
{
    if (m_renderedTarget)
        m_renderedTarget->updateLayerOrder(layerOrder);
}

void SpriteModel::onGraphicsEffectChanged(libscratchcpp::IGraphicsEffect *effect, double value)
{
}

void SpriteModel::onGraphicsEffectsCleared()
{
}

libscratchcpp::Rect SpriteModel::boundingRect() const
{
    return libscratchcpp::Rect(m_sprite->x(), m_sprite->y(), m_sprite->x(), m_sprite->y());
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
    emit renderedTargetChanged();
}

} // namespace scratchcpprender
