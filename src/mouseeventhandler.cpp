// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QMouseEvent>
#include <scratchcpp/target.h>

#include "mouseeventhandler.h"
#include "renderedtarget.h"
#include "projectloader.h"
#include "spritemodel.h"

using namespace scratchcpprender;

MouseEventHandler::MouseEventHandler(QObject *parent) :
    QObject(parent)
{
}

IRenderedTarget *MouseEventHandler::stage() const
{
    return m_stage;
}

void MouseEventHandler::setStage(IRenderedTarget *stage)
{
    m_stage = stage;
}

ProjectLoader *MouseEventHandler::projectLoader() const
{
    return m_projectLoader;
}

void MouseEventHandler::setProjectLoader(ProjectLoader *newProjectLoader)
{
    m_projectLoader = newProjectLoader;

    if (m_projectLoader) {
        connect(m_projectLoader, &ProjectLoader::spritesChanged, this, &MouseEventHandler::getSprites);
        connect(m_projectLoader, &ProjectLoader::cloneCreated, this, &MouseEventHandler::addClone);
        connect(m_projectLoader, &ProjectLoader::cloneDeleted, this, &MouseEventHandler::removeClone);
    }
}

bool MouseEventHandler::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
        case QEvent::HoverEnter:
        case QEvent::HoverLeave:
            forwardPointEvent(static_cast<QSinglePointEvent *>(event));
            return true;

        case QEvent::HoverMove:
        case QEvent::MouseMove: {
            QSinglePointEvent *pointEvent = static_cast<QSinglePointEvent *>(event);
            forwardPointEvent(pointEvent);
            QPointF pos = pointEvent->position();
            emit mouseMoved(pos.x(), pos.y());
            return true;
        }

        case QEvent::MouseButtonPress:
            emit mousePressed();
            forwardPointEvent(static_cast<QSinglePointEvent *>(event));
            return true;

        case QEvent::MouseButtonRelease: {
            emit mouseReleased();
            QQuickItem *oldClickedItem = m_clickedItem;

            if (m_clickedItem) {
                sendPointEventToItem(static_cast<QSinglePointEvent *>(event), m_clickedItem);
                m_clickedItem = nullptr;
            }

            forwardPointEvent(static_cast<QSinglePointEvent *>(event), oldClickedItem);

            return true;
        }

        case QEvent::Wheel: {
            QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
            const int delta = wheelEvent->angleDelta().y();

            if (delta > 0)
                emit mouseWheelUp();
            else if (delta < 0)
                emit mouseWheelDown();

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(obj, event);
}

void scratchcpprender::MouseEventHandler::getSprites()
{
    Q_ASSERT(m_projectLoader);

    if (!m_projectLoader)
        return;

    m_sprites.clear();
    const auto &spriteModels = m_projectLoader->spriteList();

    for (SpriteModel *model : spriteModels) {
        Q_ASSERT(model);
        IRenderedTarget *sprite = model->renderedTarget();
        Q_ASSERT(sprite);
        Q_ASSERT(sprite->scratchTarget());
        m_sprites.push_back(sprite);
    }

    // Make sure the clicked and hovered item pointers are in the list
    // If not, make them nullptr
    if (std::find(m_sprites.begin(), m_sprites.end(), m_clickedItem) == m_sprites.end())
        m_clickedItem = nullptr;

    if (std::find(m_sprites.begin(), m_sprites.end(), m_hoveredItem) == m_sprites.end())
        m_hoveredItem = nullptr;
}

void scratchcpprender::MouseEventHandler::addClone(SpriteModel *model)
{
    Q_ASSERT(model);
    IRenderedTarget *sprite = model->renderedTarget();
    Q_ASSERT(sprite);
    Q_ASSERT(std::find_if(m_sprites.begin(), m_sprites.end(), [sprite](IRenderedTarget *renderedTarget) { return renderedTarget == sprite; }) == m_sprites.end());
    m_sprites.push_back(sprite);
}

void scratchcpprender::MouseEventHandler::removeClone(SpriteModel *model)
{
    Q_ASSERT(model);
    IRenderedTarget *sprite = model->renderedTarget();
    Q_ASSERT(sprite);
    m_sprites.erase(std::remove_if(m_sprites.begin(), m_sprites.end(), [sprite](IRenderedTarget *renderedTarget) { return renderedTarget == sprite; }), m_sprites.end());
    Q_ASSERT(std::find_if(m_sprites.begin(), m_sprites.end(), [sprite](IRenderedTarget *renderedTarget) { return renderedTarget == sprite; }) == m_sprites.end());

    // Make sure the pointer is never used again after it becomes "dangling"
    if (m_clickedItem == sprite)
        m_clickedItem = nullptr;

    if (m_hoveredItem == sprite)
        m_hoveredItem = nullptr;
}

void MouseEventHandler::forwardPointEvent(QSinglePointEvent *event, QQuickItem *oldClickedItem)
{
    Q_ASSERT(m_projectLoader);

    if (!m_projectLoader)
        return;

    // Sort sprite list by layer order
    std::sort(m_sprites.begin(), m_sprites.end(), [](IRenderedTarget *t1, IRenderedTarget *t2) { return t1->scratchTarget()->layerOrder() > t2->scratchTarget()->layerOrder(); });

    // Find hovered sprite
    QQuickItem *hoveredItem = nullptr;

    for (IRenderedTarget *sprite : m_sprites) {
        // contains() expects position in the item's coordinate system
        QPointF localPos = sprite->mapFromScene(event->scenePosition());

        if (static_cast<libscratchcpp::Sprite *>(sprite->scratchTarget())->visible() && sprite->contains(localPos)) {
            hoveredItem = sprite;
            break;
        }
    }

    // If there wasn't any hovered sprite, send the event to the stage
    if (!hoveredItem) {
        hoveredItem = m_stage;
        Q_ASSERT(m_stage);
    }

    // Send the event to the item
    if (hoveredItem) {
        // Since both the hovered item and previously clicked item should receive mouse release event,
        // avoid duplicate events by checking whether the previously clicked item is the hovered item.
        if (!(event->type() == QEvent::MouseButtonRelease && hoveredItem == oldClickedItem)) {
            sendPointEventToItem(event, hoveredItem);
        }
    }
}

void MouseEventHandler::sendPointEventToItem(QSinglePointEvent *event, QQuickItem *item)
{
    Q_ASSERT(event);
    Q_ASSERT(item);
    Q_ASSERT(m_stage);

    switch (event->type()) {
        case QEvent::HoverEnter:
        case QEvent::HoverLeave:
            QCoreApplication::sendEvent(item, event);
            break;

        case QEvent::HoverMove:
            // Send HoverLeave to the previously hovered item and HoverEnter to the hovered item
            if (item != m_hoveredItem) {
                if (m_hoveredItem)
                    sendHoverEventToItem(static_cast<QHoverEvent *>(event), QEvent::HoverLeave, m_hoveredItem);

                sendHoverEventToItem(static_cast<QHoverEvent *>(event), QEvent::HoverEnter, item);
                m_hoveredItem = item;
            }

            QCoreApplication::sendEvent(item, event);
            break;

        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove: {
            if (event->type() == QEvent::MouseButtonPress)
                m_clickedItem = item;

            QMouseEvent itemEvent(
                event->type(),
                item->mapFromScene(event->scenePosition()),
                event->scenePosition(),
                event->globalPosition(),
                event->button(),
                event->buttons(),
                event->modifiers(),
                static_cast<const QPointingDevice *>(event->device()));

            QCoreApplication::sendEvent(item, &itemEvent);
            break;
        }

        default:
            Q_ASSERT(false);
            break;
    }
}

void MouseEventHandler::sendHoverEventToItem(QHoverEvent *originalEvent, QEvent::Type newType, QQuickItem *item)
{
    Q_ASSERT(newType == QEvent::HoverEnter || newType == QEvent::HoverLeave || newType == QEvent::HoverMove);

    QHoverEvent event(
        newType,
        originalEvent->scenePosition(),
        originalEvent->globalPosition(),
        originalEvent->oldPosF(),
        originalEvent->modifiers(),
        static_cast<const QPointingDevice *>(originalEvent->device()));

    QCoreApplication::sendEvent(item, &event);
}
