// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/sprite.h>

#include "scenemousearea.h"
#include "mouseeventhandler.h"

using namespace scratchcppgui;

SceneMouseArea::SceneMouseArea(QQuickItem *parent) :
    QQuickItem(parent)
{
    m_mouseHandler = new MouseEventHandler(this);
    installEventFilter(m_mouseHandler);
    connect(m_mouseHandler, &MouseEventHandler::mouseMoved, this, &SceneMouseArea::mouseMoved);
    connect(m_mouseHandler, &MouseEventHandler::mousePressed, this, &SceneMouseArea::mousePressed);
    connect(m_mouseHandler, &MouseEventHandler::mouseReleased, this, &SceneMouseArea::mouseReleased);

    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
}

IRenderedTarget *SceneMouseArea::stage() const
{
    return m_mouseHandler->stage();
}

void SceneMouseArea::setStage(IRenderedTarget *newStage)
{
    m_mouseHandler->setStage(newStage);
    emit stageChanged();
}

QQuickItem *SceneMouseArea::spriteRepeater() const
{
    return m_mouseHandler->spriteRepeater();
}

void SceneMouseArea::setSpriteRepeater(QQuickItem *newSpriteRepeater)
{
    m_mouseHandler->setSpriteRepeater(newSpriteRepeater);
    emit spriteRepeaterChanged();
}

IRenderedTarget *SceneMouseArea::draggedSprite() const
{
    return m_draggedSprite;
}

void SceneMouseArea::setDraggedSprite(IRenderedTarget *sprite)
{
    Q_ASSERT(sprite);
    m_draggedSprite = sprite;
}
