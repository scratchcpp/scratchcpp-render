// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QQuickItem>

namespace scratchcpprender
{

class IRenderedTarget;
class ProjectLoader;
class MouseEventHandler;

class SceneMouseArea : public QQuickItem
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(IRenderedTarget *stage WRITE setStage NOTIFY stageChanged)
        Q_PROPERTY(QQuickItem *spriteRepeater READ spriteRepeater WRITE setSpriteRepeater NOTIFY spriteRepeaterChanged)
        Q_PROPERTY(ProjectLoader *projectLoader READ projectLoader WRITE setProjectLoader NOTIFY projectLoaderChanged)

    public:
        explicit SceneMouseArea(QQuickItem *parent = nullptr);

        IRenderedTarget *stage() const;
        void setStage(IRenderedTarget *newStage);

        QQuickItem *spriteRepeater() const;
        void setSpriteRepeater(QQuickItem *newSpriteRepeater);

        IRenderedTarget *draggedSprite() const;
        void setDraggedSprite(IRenderedTarget *sprite);

        ProjectLoader *projectLoader() const;
        void setProjectLoader(ProjectLoader *newProjectLoader);

    signals:
        void mouseMoved(qreal x, qreal y);
        void mousePressed();
        void mouseReleased();
        void stageChanged();
        void spriteRepeaterChanged();
        void projectLoaderChanged();

    private:
        MouseEventHandler *m_mouseHandler = nullptr;
        IRenderedTarget *m_draggedSprite = nullptr;
};

} // namespace scratchcpprender
