// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/textbubble.h>

#include "targetmodel.h"
#include "renderedtarget.h"
#include "penlayer.h"
#include "graphicseffect.h"

using namespace scratchcpprender;

TargetModel::TargetModel(QObject *parent) :
    QObject(parent)
{
}

IRenderedTarget *TargetModel::renderedTarget() const
{
    return m_renderedTarget;
}

void TargetModel::setRenderedTarget(IRenderedTarget *newRenderedTarget)
{
    if (m_renderedTarget == newRenderedTarget)
        return;

    m_renderedTarget = newRenderedTarget;

    emit renderedTargetChanged();
}

IPenLayer *TargetModel::penLayer() const
{
    return m_penLayer;
}

void TargetModel::setPenLayer(IPenLayer *newPenLayer)
{
    if (m_penLayer == newPenLayer)
        return;

    m_penLayer = newPenLayer;
    emit penLayerChanged();
}

PenState &TargetModel::penState()
{
    return m_penState;
}

PenAttributes &TargetModel::penAttributes()
{
    return m_penState.penAttributes;
}

bool TargetModel::penDown() const
{
    return m_penState.penDown;
}

void TargetModel::setPenDown(bool newPenDown)
{
    m_penState.penDown = newPenDown;

    if (m_penState.penDown && m_penLayer)
        drawPenPoint(m_penLayer, m_penState.penAttributes);
}

const TextBubbleShape::Type &TargetModel::bubbleType() const
{
    return m_bubbleType;
}

const QString &TargetModel::bubbleText() const
{
    return m_bubbleText;
}

void TargetModel::setupTextBubble(libscratchcpp::TextBubble *bubble)
{
    bubble->typeChanged().connect([this](libscratchcpp::TextBubble::Type type) {
        if (type == libscratchcpp::TextBubble::Type::Say) {
            if (m_bubbleType == TextBubbleShape::Type::Say)
                return;

            m_bubbleType = TextBubbleShape::Type::Say;
        } else {
            if (m_bubbleType == TextBubbleShape::Type::Think)
                return;

            m_bubbleType = TextBubbleShape::Type::Think;
        }

        emit bubbleTypeChanged();
    });

    bubble->textChanged().connect([this](const std::string &text) {
        QString newText = QString::fromStdString(text);

        if (m_bubbleText != newText) {
            m_bubbleText = newText;
            emit bubbleTextChanged();
        }
    });

    bubble->layerOrderChanged().connect([this](int) { emit bubbleLayerChanged(); });
}

void TargetModel::updateVisibility(bool visible)
{
    if (m_renderedTarget)
        m_renderedTarget->updateVisibility(visible);
}

void TargetModel::updateX(double x)
{
    if (m_renderedTarget)
        m_renderedTarget->updateX(x);
}

void TargetModel::updateY(double y)
{
    if (m_renderedTarget)
        m_renderedTarget->updateY(y);
}

void TargetModel::updateSize(double size)
{
    if (m_renderedTarget)
        m_renderedTarget->updateSize(size);
}

void TargetModel::updateDirection(double direction)
{
    if (m_renderedTarget)
        m_renderedTarget->updateDirection(direction);
}

void TargetModel::updateRotationStyle(libscratchcpp::Sprite::RotationStyle style)
{
    if (m_renderedTarget)
        m_renderedTarget->updateRotationStyle(style);
}

void TargetModel::updateLayerOrder(int layerOrder)
{
    if (m_renderedTarget)
        m_renderedTarget->updateLayerOrder(layerOrder);
}

void TargetModel::updateCostume(libscratchcpp::Costume *costume)
{
    if (m_renderedTarget)
        m_renderedTarget->updateCostume(costume);
}

void TargetModel::onMoved(double oldX, double oldY, double newX, double newY)
{
    if (m_penState.penDown && m_penLayer)
        drawPenLine(m_penLayer, m_penState.penAttributes, oldX, oldY, newX, newY);
}

void TargetModel::setGraphicEffect(libscratchcpp::IGraphicsEffect *effect, double value)
{
    GraphicsEffect *graphicsEffect = dynamic_cast<GraphicsEffect *>(effect);

    if (graphicsEffect && m_renderedTarget)
        m_renderedTarget->setGraphicEffect(graphicsEffect->effect(), value);
}

void TargetModel::clearGraphicEffects()
{
    if (m_renderedTarget)
        m_renderedTarget->clearGraphicEffects();
}

int TargetModel::costumeWidth() const
{
    return m_renderedTarget->costumeWidth();
}

int TargetModel::costumeHeight() const
{
    return m_renderedTarget->costumeHeight();
}

void TargetModel::getBoundingRect(libscratchcpp::Rect &dst) const
{
    dst = m_renderedTarget->getBounds();
}

void TargetModel::getFastBoundingRect(libscratchcpp::Rect &dst) const
{
    dst = m_renderedTarget->getFastBounds();
}

bool TargetModel::touchingClones(const std::vector<libscratchcpp::Sprite *> &clones) const
{
    return m_renderedTarget->touchingClones(clones);
}

bool TargetModel::touchingPoint(double x, double y) const
{
    return m_renderedTarget->containsScratchPoint(x, y);
}

bool TargetModel::touchingColor(libscratchcpp::Rgb color) const
{
    return m_renderedTarget->touchingColor(color);
}

bool TargetModel::touchingColor(libscratchcpp::Rgb color, libscratchcpp::Rgb mask) const
{
    return m_renderedTarget->touchingColor(color, mask);
}
