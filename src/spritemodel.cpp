// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/costume.h>
#include <scratchcpp/rect.h>
#include <scratchcpp/iengine.h>

#include "spritemodel.h"
#include "renderedtarget.h"
#include "ipenlayer.h"
#include "graphicseffect.h"

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
    cloneModel->m_penLayer = m_penLayer;
    cloneModel->m_penState = m_penState;
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

void SpriteModel::onMoved(double oldX, double oldY, double newX, double newY)
{
    if (m_penState.penDown && m_penLayer) {
        m_penLayer->drawLine(m_penState.penAttributes, oldX, oldY, newX, newY);
        libscratchcpp::IEngine *engine = m_sprite->engine();

        if (engine)
            engine->requestRedraw();
    }
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
    GraphicsEffect *graphicsEffect = dynamic_cast<GraphicsEffect *>(effect);

    if (graphicsEffect && m_renderedTarget)
        m_renderedTarget->setGraphicEffect(graphicsEffect->effect(), value);
}

void SpriteModel::onGraphicsEffectsCleared()
{
    if (m_renderedTarget)
        m_renderedTarget->clearGraphicEffects();
}

void SpriteModel::onBubbleTypeChanged(libscratchcpp::Target::BubbleType type)
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

void SpriteModel::onBubbleTextChanged(const std::string &text)
{
    QString newText = QString::fromStdString(text);

    if (m_bubbleText != newText) {
        m_bubbleText = newText;
        emit bubbleTextChanged();
    }
}

libscratchcpp::Rect SpriteModel::boundingRect() const
{
    return m_renderedTarget->getBounds();
}

libscratchcpp::Rect SpriteModel::fastBoundingRect() const
{
    return m_renderedTarget->getFastBounds();
}

bool SpriteModel::touchingClones(const std::vector<libscratchcpp::Sprite *> &clones) const
{
    return m_renderedTarget->touchingClones(clones);
}

bool SpriteModel::touchingPoint(double x, double y) const
{
    return m_renderedTarget->containsScratchPoint(x, y);
}

bool SpriteModel::touchingColor(const libscratchcpp::Value &color) const
{
    return false;
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

IPenLayer *SpriteModel::penLayer() const
{
    return m_penLayer;
}

void SpriteModel::setPenLayer(IPenLayer *newPenLayer)
{
    if (m_penLayer == newPenLayer)
        return;

    m_penLayer = newPenLayer;
    emit penLayerChanged();
}

PenState &SpriteModel::penState()
{
    return m_penState;
}

PenAttributes &SpriteModel::penAttributes()
{
    return m_penState.penAttributes;
}

bool SpriteModel::penDown() const
{
    return m_penState.penDown;
}

void SpriteModel::setPenDown(bool newPenDown)
{
    m_penState.penDown = newPenDown;

    if (m_penState.penDown && m_penLayer && m_sprite) {
        m_penLayer->drawPoint(m_penState.penAttributes, m_sprite->x(), m_sprite->y());
        libscratchcpp::IEngine *engine = m_sprite->engine();

        if (engine)
            engine->requestRedraw();
    }
}

SpriteModel *SpriteModel::cloneRoot() const
{
    if (m_cloneRoot == this)
        return nullptr;
    else
        return m_cloneRoot;
}

const TextBubbleShape::Type &SpriteModel::bubbleType() const
{
    return m_bubbleType;
}

const QString &SpriteModel::bubbleText() const
{
    return m_bubbleText;
}

} // namespace scratchcpprender
