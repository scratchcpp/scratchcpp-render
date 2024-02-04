// SPDX-License-Identifier: LGPL-3.0-or-later

#include "textbubbleshape.h"
#include "textbubblepainter.h"

using namespace scratchcpprender;

TextBubbleShape::TextBubbleShape(QQuickItem *parent) :
    QNanoQuickItem(parent)
{
}

QNanoQuickItemPainter *TextBubbleShape::createItemPainter() const
{
    return new TextBubblePainter;
}

TextBubbleShape::Type TextBubbleShape::type() const
{
    return m_type;
}

void TextBubbleShape::setType(Type newType)
{
    if (m_type == newType)
        return;

    m_type = newType;
    update();
    emit typeChanged();
}

bool TextBubbleShape::onSpriteRight() const
{
    return m_onSpriteRight;
}

void TextBubbleShape::setOnSpriteRight(bool newOnSpriteRight)
{
    if (m_onSpriteRight == newOnSpriteRight)
        return;

    m_onSpriteRight = newOnSpriteRight;
    update();
    emit onSpriteRightChanged();
}

double TextBubbleShape::stageScale() const
{
    return m_stageScale;
}

void TextBubbleShape::setStageScale(double newStageScale)
{
    if (qFuzzyCompare(m_stageScale, newStageScale))
        return;

    m_stageScale = newStageScale;
    setWidth(m_nativeWidth * m_stageScale);
    setHeight(m_nativeHeight * m_stageScale);
    emit stageScaleChanged();
}

double TextBubbleShape::nativeWidth() const
{
    return m_nativeWidth;
}

void TextBubbleShape::setNativeWidth(double newNativeWidth)
{
    if (qFuzzyCompare(m_nativeWidth, newNativeWidth))
        return;

    m_nativeWidth = newNativeWidth;
    setWidth(m_nativeWidth * m_stageScale);
    emit nativeWidthChanged();
}

double TextBubbleShape::nativeHeight() const
{
    return m_nativeHeight;
}

void TextBubbleShape::setNativeHeight(double newNativeHeight)
{
    if (qFuzzyCompare(m_nativeHeight, newNativeHeight))
        return;

    m_nativeHeight = newNativeHeight;
    setHeight(m_nativeHeight * m_stageScale);
    emit nativeHeightChanged();
}
