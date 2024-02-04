// SPDX-License-Identifier: LGPL-3.0-or-later

#include "textbubblepainter.h"
#include "textbubbleshape.h"

using namespace scratchcpprender;

// https://github.com/scratchfoundation/scratch-render/blob/ac935423afe3ba79235750eecb1e443474c6eb09/src/TextBubbleSkin.js#L7-L26
static const int STROKE_WIDTH = 4;
static const int CORNER_RADIUS = 16;
static const int TAIL_HEIGHT = 12;
static const QNanoColor BUBBLE_FILL_COLOR = QNanoColor(255, 255, 255);
static const QNanoColor BUBBLE_STROKE_COLOR = QNanoColor(0, 0, 0, 38);

static const double pi = std::acos(-1); // TODO: Use std::numbers::pi in C++20

void TextBubblePainter::paint(QNanoPainter *painter)
{
    // https://github.com/scratchfoundation/scratch-render/blob/ac935423afe3ba79235750eecb1e443474c6eb09/src/TextBubbleSkin.js#L149-L242
    if (!m_item)
        return;

    const double scale = m_item->stageScale();
    const double width = m_item->nativeWidth() - STROKE_WIDTH;
    const double height = m_item->nativeHeight() - STROKE_WIDTH - TAIL_HEIGHT;

    painter->resetTransform();
    painter->scale(scale, scale);
    painter->translate(STROKE_WIDTH * 0.5, STROKE_WIDTH * 0.5);

    // If the text bubble points leftward, flip the canvas
    painter->save();

    if (m_item->onSpriteRight()) {
        painter->scale(-1, 1);
        painter->translate(-width, 0);
    }

    // Draw the bubble's rounded borders
    painter->beginPath();
    painter->moveTo(CORNER_RADIUS, height);
    painter->arcTo(0, height, 0, height - CORNER_RADIUS, CORNER_RADIUS);
    painter->arcTo(0, 0, width, 0, CORNER_RADIUS);
    painter->arcTo(width, 0, width, height, CORNER_RADIUS);
    painter->arcTo(width, height, width - CORNER_RADIUS, height, CORNER_RADIUS);

    // Translate the canvas so we don't have to do a bunch of width/height arithmetic
    painter->save();
    painter->translate(width - CORNER_RADIUS, height);

    // Draw the bubble's "tail"
    if (m_item->type() == TextBubbleShape::Type::Say) {
        // For a speech bubble, draw one swoopy thing
        painter->bezierTo(0, 4, 4, 8, 4, 10);
        painter->arcTo(4, 12, 2, 12, 2);
        painter->bezierTo(-1, 12, -11, 8, -16, 0);

        painter->closePath();
    } else {
        // For a thinking bubble, draw a partial circle attached to the bubble...
        painter->arc(-16, 0, 4, 0, pi);

        painter->closePath();

        // and two circles detached from it
        painter->moveTo(-7, 7.25);
        painter->arc(-9.25, 7.25, 2.25, 0, pi * 2);

        painter->moveTo(0, 9.5);
        painter->arc(-1.5, 9.5, 1.5, 0, pi * 2);
    }

    // Un-translate the canvas and fill + stroke the text bubble
    painter->restore();

    painter->setFillStyle(BUBBLE_FILL_COLOR);
    painter->setStrokeStyle(BUBBLE_STROKE_COLOR);
    painter->setLineWidth(STROKE_WIDTH);

    painter->stroke();
    painter->fill();
}

void TextBubblePainter::synchronize(QNanoQuickItem *item)
{
    Q_ASSERT(item);
    m_item = static_cast<TextBubbleShape *>(item);
}
