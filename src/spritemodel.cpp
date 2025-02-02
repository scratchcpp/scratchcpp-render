// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/costume.h>
#include <scratchcpp/rect.h>
#include <scratchcpp/iengine.h>
#include <scratchcpp/textbubble.h>

#include "spritemodel.h"
#include "penlayer.h"

using namespace scratchcpprender;

SpriteModel::SpriteModel(QObject *parent) :
    TargetModel(parent)
{
}

void SpriteModel::init(libscratchcpp::Sprite *sprite)
{
    m_sprite = sprite;

    if (m_sprite)
        setupTextBubble(m_sprite->bubble());
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
    cloneModel->setPenLayer(penLayer());
    cloneModel->penState() = penState();
    clone->setInterface(cloneModel);
    emit cloned(cloneModel);
}

void SpriteModel::onCostumeChanged(libscratchcpp::Costume *costume)
{
    updateCostume(costume);
}

void SpriteModel::onVisibleChanged(bool visible)
{
    updateVisibility(visible);
}

void SpriteModel::onXChanged(double x)
{
    updateX(x);
}

void SpriteModel::onYChanged(double y)
{
    updateY(y);
}

void SpriteModel::onMoved(double oldX, double oldY, double newX, double newY)
{
    TargetModel::onMoved(oldX, oldY, newX, newY);
}

void SpriteModel::onSizeChanged(double size)
{
    updateSize(size);
}

void SpriteModel::onDirectionChanged(double direction)
{
    updateDirection(direction);
}

void SpriteModel::onRotationStyleChanged(libscratchcpp::Sprite::RotationStyle rotationStyle)
{
    updateRotationStyle(rotationStyle);
}

void SpriteModel::onLayerOrderChanged(int layerOrder)
{
    updateLayerOrder(layerOrder);
}

void SpriteModel::onGraphicsEffectChanged(libscratchcpp::IGraphicsEffect *effect, double value)
{
    setGraphicEffect(effect, value);
}

void SpriteModel::onGraphicsEffectsCleared()
{
    clearGraphicEffects();
}

int SpriteModel::costumeWidth() const
{
    return TargetModel::costumeWidth();
}

int SpriteModel::costumeHeight() const
{
    return TargetModel::costumeHeight();
}

libscratchcpp::Rect SpriteModel::boundingRect() const
{
    libscratchcpp::Rect ret;
    getBoundingRect(ret);
    return ret;
}

libscratchcpp::Rect SpriteModel::fastBoundingRect() const
{
    libscratchcpp::Rect ret;
    getFastBoundingRect(ret);
    return ret;
}

bool SpriteModel::touchingClones(const std::vector<libscratchcpp::Sprite *> &clones) const
{
    return TargetModel::touchingClones(clones);
}

bool SpriteModel::touchingPoint(double x, double y) const
{
    return TargetModel::touchingPoint(x, y);
}

bool SpriteModel::touchingColor(libscratchcpp::Rgb color) const
{
    return TargetModel::touchingColor(color);
}

bool SpriteModel::touchingColor(libscratchcpp::Rgb color, libscratchcpp::Rgb mask) const
{
    return TargetModel::touchingColor(color, mask);
}

libscratchcpp::Sprite *SpriteModel::sprite() const
{
    return m_sprite;
}

int SpriteModel::bubbleLayer() const
{
    return m_sprite ? m_sprite->bubble()->layerOrder() : 0;
}

SpriteModel *SpriteModel::cloneRoot() const
{
    if (m_cloneRoot == this)
        return nullptr;
    else
        return m_cloneRoot;
}

void SpriteModel::loadCostume()
{
    if (m_sprite)
        updateCostume(m_sprite->currentCostume().get());
}

void SpriteModel::drawPenPoint(IPenLayer *penLayer, const PenAttributes &penAttributes)
{
    penLayer->drawPoint(penAttributes, m_sprite->x(), m_sprite->y());
    libscratchcpp::IEngine *engine = m_sprite->engine();

    if (engine)
        engine->requestRedraw();
}

void SpriteModel::drawPenLine(IPenLayer *penLayer, const PenAttributes &penAttributes, double x0, double y0, double x1, double y1)
{
    penLayer->drawLine(penAttributes, x0, y0, x1, y1);
    libscratchcpp::IEngine *engine = m_sprite->engine();

    if (engine)
        engine->requestRedraw();
}
