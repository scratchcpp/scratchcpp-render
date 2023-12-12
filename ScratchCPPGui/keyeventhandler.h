// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QObject>

namespace scratchcppgui
{

class KeyEventHandler : public QObject
{
        Q_OBJECT
    public:
        explicit KeyEventHandler(QObject *parent = nullptr);

    signals:
        void keyPressed(Qt::Key key, const QString &text);
        void keyReleased(Qt::Key key, const QString &text);

    protected:
        bool eventFilter(QObject *obj, QEvent *event) override;
};

} // namespace scratchcppgui
