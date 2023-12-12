// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QQuickPaintedItem>

namespace libscratchcpp
{
class IEngine;
}

namespace scratchcppgui
{

class KeyEventHandler;

class ProjectScene : public QQuickItem
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(libscratchcpp::IEngine *engine READ engine WRITE setEngine NOTIFY engineChanged)

    public:
        ProjectScene(QQuickItem *parent = nullptr);

        libscratchcpp::IEngine *engine() const;
        void setEngine(libscratchcpp::IEngine *newEngine);

        Q_INVOKABLE void handleMouseMove(qreal x, qreal y);
        Q_INVOKABLE void handleMousePress();
        Q_INVOKABLE void handleMouseRelease();

        void handleKeyPress(Qt::Key key, const QString &text);
        void handleKeyRelease(Qt::Key key, const QString &text);

    signals:
        void engineChanged();

    private:
        void installKeyHandler(QQuickWindow *window);

        libscratchcpp::IEngine *m_engine = nullptr;
        KeyEventHandler *m_keyHandler = nullptr;
};

} // namespace scratchcppgui
