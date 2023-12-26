// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QKeyEvent>

#include "keyeventhandler.h"

using namespace scratchcppgui;

KeyEventHandler::KeyEventHandler(QObject *parent) :
    QObject(parent)
{
}

bool KeyEventHandler::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
        case QEvent::KeyPress: {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            emit keyPressed(static_cast<Qt::Key>(keyEvent->key()), keyEvent->text());
            break;
        }

        case QEvent::KeyRelease: {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            emit keyReleased(static_cast<Qt::Key>(keyEvent->key()), keyEvent->text());
            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(obj, event);
}
