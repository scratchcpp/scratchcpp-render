// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/sprite.h>

#include "scenemousearea.h"
#include "mouseeventhandler.h"

using namespace scratchcpprender;

SceneMouseArea::SceneMouseArea(QQuickItem *parent) :
    QQuickItem(parent)
{
    m_mouseHandler = new MouseEventHandler(this);
    installEventFilter(m_mouseHandler);
    connect(m_mouseHandler, &MouseEventHandler::mouseMoved, this, &SceneMouseArea::mouseMoved);
    connect(m_mouseHandler, &MouseEventHandler::mousePressed, this, &SceneMouseArea::mousePressed);
    connect(m_mouseHandler, &MouseEventHandler::mouseReleased, this, &SceneMouseArea::mouseReleased);
    connect(m_mouseHandler, &MouseEventHandler::mouseWheelUp, this, &SceneMouseArea::mouseWheelUp);
    connect(m_mouseHandler, &MouseEventHandler::mouseWheelDown, this, &SceneMouseArea::mouseWheelDown);

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

IRenderedTarget *SceneMouseArea::draggedSprite() const
{
    return m_draggedSprite;
}

void SceneMouseArea::setDraggedSprite(IRenderedTarget *sprite)
{
    m_draggedSprite = sprite;
}

ProjectLoader *SceneMouseArea::projectLoader() const
{
    return m_mouseHandler->projectLoader();
}

void SceneMouseArea::setProjectLoader(ProjectLoader *newProjectLoader)
{
    m_mouseHandler->setProjectLoader(newProjectLoader);
    emit projectLoaderChanged();
}
