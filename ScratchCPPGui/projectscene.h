// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QQuickPaintedItem>

namespace scratchcppgui
{

class ProjectScene : public QQuickItem
{
        Q_OBJECT
        QML_ELEMENT

    public:
        ProjectScene(QQuickItem *parent = nullptr);
};

} // namespace scratchcppgui
