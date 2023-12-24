#include <QtTest/QSignalSpy>
#include <QKeyEvent>
#include <keyeventhandler.h>

#include "../common.h"

using namespace scratchcppgui;

TEST(KeyEventHandlerTest, EventFilter)
{
    KeyEventHandler handler;
    QSignalSpy keyPressedSpy(&handler, &KeyEventHandler::keyPressed);
    QSignalSpy keyReleasedSpy(&handler, &KeyEventHandler::keyReleased);

    {
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Space, Qt::ShiftModifier | Qt::AltModifier, " ", true, 1);
        handler.eventFilter(nullptr, &event);
        ASSERT_EQ(keyPressedSpy.count(), 1);
        ASSERT_EQ(keyReleasedSpy.count(), 0);
        keyPressedSpy.clear();
        keyReleasedSpy.clear();
    }

    {
        QKeyEvent event(QEvent::KeyRelease, Qt::Key_Space, Qt::ShiftModifier | Qt::AltModifier, " ", true, 1);
        handler.eventFilter(nullptr, &event);
        ASSERT_EQ(keyPressedSpy.count(), 0);
        ASSERT_EQ(keyReleasedSpy.count(), 1);
        keyPressedSpy.clear();
        keyReleasedSpy.clear();
    }
}
