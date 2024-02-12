// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QObject>
#include <QEvent>

class QQuickItem;
class QSinglePointEvent;
class QHoverEvent;

namespace scratchcpprender
{

class IRenderedTarget;
class ProjectLoader;
class SpriteModel;

class MouseEventHandler : public QObject
{
        Q_OBJECT
    public:
        explicit MouseEventHandler(QObject *parent = nullptr);

        IRenderedTarget *stage() const;
        void setStage(IRenderedTarget *stage);

        bool eventFilter(QObject *obj, QEvent *event) override;

        ProjectLoader *projectLoader() const;
        void setProjectLoader(ProjectLoader *newProjectLoader);

    signals:
        void mouseMoved(qreal x, qreal y);
        void mousePressed();
        void mouseReleased();
        void mouseWheelUp();
        void mouseWheelDown();

    private:
        void getSprites();
        void addClone(SpriteModel *model);
        void removeClone(SpriteModel *model);
        void forwardPointEvent(QSinglePointEvent *event, QQuickItem *oldClickedItem = nullptr);
        void sendPointEventToItem(QSinglePointEvent *event, QQuickItem *item);
        void sendHoverEventToItem(QHoverEvent *originalEvent, QEvent::Type newType, QQuickItem *item);

        IRenderedTarget *m_stage = nullptr;
        std::vector<IRenderedTarget *> m_sprites;
        QQuickItem *m_hoveredItem = nullptr;
        QQuickItem *m_clickedItem = nullptr;
        ProjectLoader *m_projectLoader = nullptr;
};

} // namespace scratchcpprender
