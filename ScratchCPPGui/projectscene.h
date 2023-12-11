// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QQuickPaintedItem>

namespace libscratchcpp
{
class IEngine;
}

namespace scratchcppgui
{

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

    signals:
        void engineChanged();

    private:
        libscratchcpp::IEngine *m_engine = nullptr;
};

} // namespace scratchcppgui
