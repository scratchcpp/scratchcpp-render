#include <QtTest/QSignalSpy>
#include <scratchcpp/sprite.h>
#include <mouseeventhandler.h>
#include <renderedtargetmock.h>

#include "../common.h"
#include "fakerepeater.h"

using namespace scratchcppgui;
using namespace libscratchcpp;

using ::testing::Return;
using ::testing::WithArgs;
using ::testing::Invoke;
using ::testing::_;

TEST(SceneMouseAreaTest, Constructors)
{
    MouseEventHandler handler1;
    MouseEventHandler handler2(&handler1);
    ASSERT_EQ(handler2.parent(), &handler1);
}

TEST(MouseEventHandlerTest, Stage)
{
    MouseEventHandler handler;
    ASSERT_EQ(handler.stage(), nullptr);

    RenderedTargetMock stage;
    handler.setStage(&stage);
    ASSERT_EQ(handler.stage(), &stage);
}

TEST(MouseEventHandlerTest, SpriteRepeater)
{
    MouseEventHandler handler;
    ASSERT_EQ(handler.spriteRepeater(), nullptr);

    QQuickItem item;
    handler.setSpriteRepeater(&item);
    ASSERT_EQ(handler.spriteRepeater(), &item);
}

TEST(MouseEventHandlerTest, HoverEnterLeaveEvent)
{
    MouseEventHandler handler;
    RenderedTargetMock stage, renderedTarget1, renderedTarget2, renderedTarget3;
    Sprite sprite1, sprite2, sprite3;
    sprite1.setLayerOrder(2);
    sprite2.setLayerOrder(1);
    sprite3.setLayerOrder(3);
    FakeRepeater repeater;
    repeater.items = { &renderedTarget1, &renderedTarget2, &renderedTarget3 };
    handler.setStage(&stage);
    handler.setSpriteRepeater(&repeater);
    QPointingDevice dev;

    static const QPointF localPos(8.5, 2.9);
    static const QPointF scenePos(10.5, 4.9);
    static const QPointF globalPos(11.5, 5.9);
    static const QPointF oldPos(9.5, 3.9);

    EXPECT_CALL(renderedTarget1, scratchTarget()).WillRepeatedly(Return(&sprite1));
    EXPECT_CALL(renderedTarget2, scratchTarget()).WillRepeatedly(Return(&sprite2));
    EXPECT_CALL(renderedTarget3, scratchTarget()).WillRepeatedly(Return(&sprite3));

    EXPECT_CALL(renderedTarget1, mapFromScene(scenePos)).WillRepeatedly(Return(localPos));
    EXPECT_CALL(renderedTarget2, mapFromScene(scenePos)).WillRepeatedly(Return(localPos));
    EXPECT_CALL(renderedTarget3, mapFromScene(scenePos)).WillRepeatedly(Return(localPos));

    {
        // HoverEnter
        QHoverEvent event(QEvent::HoverEnter, scenePos, globalPos, oldPos, Qt::NoModifier, &dev);

        EXPECT_CALL(renderedTarget3, contains(localPos)).WillOnce(Return(false));
        EXPECT_CALL(renderedTarget1, contains(localPos)).WillOnce(Return(true));
        EXPECT_CALL(renderedTarget1, hoverEnterEvent(&event));
        ASSERT_TRUE(handler.eventFilter(nullptr, &event));

        EXPECT_CALL(renderedTarget3, contains(localPos)).WillOnce(Return(false));
        EXPECT_CALL(renderedTarget1, contains(localPos)).WillOnce(Return(false));
        EXPECT_CALL(renderedTarget2, contains(localPos)).WillOnce(Return(false));
        EXPECT_CALL(stage, hoverEnterEvent(&event));
        ASSERT_TRUE(handler.eventFilter(nullptr, &event));
    }

    {
        // HoverLeave
        QHoverEvent event(QEvent::HoverLeave, scenePos, globalPos, oldPos, Qt::NoModifier, &dev);

        EXPECT_CALL(renderedTarget3, contains(localPos)).WillOnce(Return(false));
        EXPECT_CALL(renderedTarget1, contains(localPos)).WillOnce(Return(false));
        EXPECT_CALL(renderedTarget2, contains(localPos)).WillOnce(Return(true));
        EXPECT_CALL(renderedTarget2, hoverLeaveEvent(&event));
        ASSERT_TRUE(handler.eventFilter(nullptr, &event));

        EXPECT_CALL(renderedTarget3, contains(localPos)).WillOnce(Return(false));
        EXPECT_CALL(renderedTarget1, contains(localPos)).WillOnce(Return(false));
        EXPECT_CALL(renderedTarget2, contains(localPos)).WillOnce(Return(false));
        EXPECT_CALL(stage, hoverLeaveEvent(&event));
        ASSERT_TRUE(handler.eventFilter(nullptr, &event));
    }
}

TEST(MouseEventHandlerTest, HoverMoveEvent)
{
    MouseEventHandler handler;
    RenderedTargetMock stage, renderedTarget1, renderedTarget2, renderedTarget3;
    Sprite sprite1, sprite2, sprite3;
    sprite1.setLayerOrder(2);
    sprite2.setLayerOrder(1);
    sprite3.setLayerOrder(3);
    FakeRepeater repeater;
    repeater.items = { &renderedTarget1, &renderedTarget2, &renderedTarget3 };
    handler.setStage(&stage);
    handler.setSpriteRepeater(&repeater);
    QPointingDevice dev;

    static const QPointF localPos(8.5, 2.9);
    static const QPointF scenePos(10.5, 4.9);
    static const QPointF globalPos(11.5, 5.9);
    static const QPointF oldPos(9.5, 3.9);

    EXPECT_CALL(renderedTarget1, scratchTarget()).WillRepeatedly(Return(&sprite1));
    EXPECT_CALL(renderedTarget2, scratchTarget()).WillRepeatedly(Return(&sprite2));
    EXPECT_CALL(renderedTarget3, scratchTarget()).WillRepeatedly(Return(&sprite3));

    EXPECT_CALL(renderedTarget1, mapFromScene(scenePos)).WillRepeatedly(Return(localPos));
    EXPECT_CALL(renderedTarget2, mapFromScene(scenePos)).WillRepeatedly(Return(localPos));
    EXPECT_CALL(renderedTarget3, mapFromScene(scenePos)).WillRepeatedly(Return(localPos));

    QHoverEvent event(QEvent::HoverMove, scenePos, globalPos, oldPos, Qt::ShiftModifier, &dev);
    QSignalSpy spy(&handler, &MouseEventHandler::mouseMoved);

    auto checkEnterLeaveEvent = [&dev](QHoverEvent *event, bool enter) {
        ASSERT_TRUE(event);
        ASSERT_EQ(event->type(), enter ? QEvent::HoverEnter : QEvent::HoverLeave);
        ASSERT_EQ(event->scenePosition(), scenePos);
        ASSERT_EQ(event->globalPosition(), globalPos);
        ASSERT_EQ(event->oldPosF(), oldPos);
        ASSERT_EQ(event->modifiers(), Qt::ShiftModifier);
        ASSERT_EQ(event->device(), &dev);
    };

    auto checkEnterEvent = [checkEnterLeaveEvent](QHoverEvent *event) { checkEnterLeaveEvent(event, true); };
    auto checkLeaveEvent = [checkEnterLeaveEvent](QHoverEvent *event) { checkEnterLeaveEvent(event, false); };

    // Send to sprite 1
    EXPECT_CALL(renderedTarget3, contains(localPos)).WillOnce(Return(false));
    EXPECT_CALL(renderedTarget1, contains(localPos)).WillOnce(Return(true));
    EXPECT_CALL(renderedTarget1, hoverEnterEvent(_)).WillOnce(WithArgs<0>(Invoke(checkEnterEvent)));
    EXPECT_CALL(renderedTarget1, hoverMoveEvent(&event));
    ASSERT_TRUE(handler.eventFilter(nullptr, &event));
    ASSERT_EQ(spy.count(), 1);
    spy.clear();

    // Send to sprite 2
    EXPECT_CALL(renderedTarget3, contains(localPos)).WillOnce(Return(false));
    EXPECT_CALL(renderedTarget1, contains(localPos)).WillOnce(Return(false));
    EXPECT_CALL(renderedTarget2, contains(localPos)).WillOnce(Return(true));
    EXPECT_CALL(renderedTarget1, hoverLeaveEvent(_)).WillOnce(WithArgs<0>(Invoke(checkLeaveEvent)));
    EXPECT_CALL(renderedTarget2, hoverEnterEvent(_)).WillOnce(WithArgs<0>(Invoke(checkEnterEvent)));
    EXPECT_CALL(renderedTarget2, hoverMoveEvent(&event));
    ASSERT_TRUE(handler.eventFilter(nullptr, &event));
    ASSERT_EQ(spy.count(), 1);
    spy.clear();

    // Send to stage
    EXPECT_CALL(renderedTarget3, contains(localPos)).WillOnce(Return(false));
    EXPECT_CALL(renderedTarget1, contains(localPos)).WillOnce(Return(false));
    EXPECT_CALL(renderedTarget2, contains(localPos)).WillOnce(Return(false));
    EXPECT_CALL(renderedTarget2, hoverLeaveEvent(_)).WillOnce(WithArgs<0>(Invoke(checkLeaveEvent)));
    EXPECT_CALL(stage, hoverEnterEvent(_)).WillOnce(WithArgs<0>(Invoke(checkEnterEvent)));
    EXPECT_CALL(stage, hoverMoveEvent(&event));
    ASSERT_TRUE(handler.eventFilter(nullptr, &event));
    ASSERT_EQ(spy.count(), 1);
    spy.clear();

    // Send to sprite 3
    EXPECT_CALL(renderedTarget3, contains(localPos)).WillOnce(Return(true));
    EXPECT_CALL(stage, hoverLeaveEvent(_)).WillOnce(WithArgs<0>(Invoke(checkLeaveEvent)));
    EXPECT_CALL(renderedTarget3, hoverEnterEvent(_)).WillOnce(WithArgs<0>(Invoke(checkEnterEvent)));
    EXPECT_CALL(renderedTarget3, hoverMoveEvent(&event));
    ASSERT_TRUE(handler.eventFilter(nullptr, &event));
    ASSERT_EQ(spy.count(), 1);
    spy.clear();
}

TEST(MouseEventHandlerTest, MouseMoveEvent)
{
    MouseEventHandler handler;
    RenderedTargetMock stage, renderedTarget1, renderedTarget2, renderedTarget3;
    Sprite sprite1, sprite2, sprite3;
    sprite1.setLayerOrder(2);
    sprite2.setLayerOrder(1);
    sprite3.setLayerOrder(3);
    FakeRepeater repeater;
    repeater.items = { &renderedTarget1, &renderedTarget2, &renderedTarget3 };
    handler.setStage(&stage);
    handler.setSpriteRepeater(&repeater);
    QPointingDevice dev;

    static const QPointF localPos(8.5, 2.9);
    static const QPointF scenePos(10.5, 4.9);
    static const QPointF globalPos(11.5, 5.9);
    static const QPointF oldPos(9.5, 3.9);

    EXPECT_CALL(renderedTarget1, scratchTarget()).WillRepeatedly(Return(&sprite1));
    EXPECT_CALL(renderedTarget2, scratchTarget()).WillRepeatedly(Return(&sprite2));
    EXPECT_CALL(renderedTarget3, scratchTarget()).WillRepeatedly(Return(&sprite3));

    EXPECT_CALL(renderedTarget1, mapFromScene(scenePos)).WillRepeatedly(Return(localPos));
    EXPECT_CALL(renderedTarget2, mapFromScene(scenePos)).WillRepeatedly(Return(localPos));
    EXPECT_CALL(renderedTarget3, mapFromScene(scenePos)).WillRepeatedly(Return(localPos));

    QMouseEvent event(QEvent::MouseMove, scenePos, scenePos, globalPos, Qt::LeftButton, Qt::LeftButton | Qt::RightButton, Qt::ShiftModifier, &dev);
    QSignalSpy spy(&handler, &MouseEventHandler::mouseMoved);

    auto checkMoveEvent = [&dev](QMouseEvent *event) {
        ASSERT_TRUE(event);
        ASSERT_EQ(event->type(), QEvent::MouseMove);
        ASSERT_EQ(event->position(), scenePos);
        ASSERT_EQ(event->scenePosition(), scenePos);
        ASSERT_EQ(event->globalPosition(), globalPos);
        ASSERT_EQ(event->button(), Qt::LeftButton);
        ASSERT_EQ(event->buttons(), Qt::LeftButton | Qt::RightButton);
        ASSERT_EQ(event->modifiers(), Qt::ShiftModifier);
        ASSERT_EQ(event->device(), &dev);
    };

    // Send to sprite 1
    EXPECT_CALL(renderedTarget3, contains(localPos)).WillOnce(Return(false));
    EXPECT_CALL(renderedTarget1, contains(localPos)).WillOnce(Return(true));
    EXPECT_CALL(renderedTarget1, mouseMoveEvent(_)).WillOnce(WithArgs<0>(Invoke(checkMoveEvent)));
    ASSERT_TRUE(handler.eventFilter(nullptr, &event));
    ASSERT_EQ(spy.count(), 1);
    spy.clear();

    // Send to stage
    EXPECT_CALL(renderedTarget3, contains(localPos)).WillOnce(Return(false));
    EXPECT_CALL(renderedTarget1, contains(localPos)).WillOnce(Return(false));
    EXPECT_CALL(renderedTarget2, contains(localPos)).WillOnce(Return(false));
    EXPECT_CALL(stage, mouseMoveEvent(_)).WillOnce(WithArgs<0>(Invoke(checkMoveEvent)));
    ASSERT_TRUE(handler.eventFilter(nullptr, &event));
    ASSERT_EQ(spy.count(), 1);
    spy.clear();
}

TEST(MouseEventHandlerTest, MousePressReleaseEvent)
{
    MouseEventHandler handler;
    RenderedTargetMock stage, renderedTarget1, renderedTarget2, renderedTarget3;
    Sprite sprite1, sprite2, sprite3;
    sprite1.setLayerOrder(2);
    sprite2.setLayerOrder(1);
    sprite3.setLayerOrder(3);
    FakeRepeater repeater;
    repeater.items = { &renderedTarget1, &renderedTarget2, &renderedTarget3 };
    handler.setStage(&stage);
    handler.setSpriteRepeater(&repeater);
    QPointingDevice dev;

    static const QPointF localPos(8.5, 2.9);
    static const QPointF scenePos(10.5, 4.9);
    static const QPointF globalPos(11.5, 5.9);
    static const QPointF oldPos(9.5, 3.9);

    EXPECT_CALL(renderedTarget1, scratchTarget()).WillRepeatedly(Return(&sprite1));
    EXPECT_CALL(renderedTarget2, scratchTarget()).WillRepeatedly(Return(&sprite2));
    EXPECT_CALL(renderedTarget3, scratchTarget()).WillRepeatedly(Return(&sprite3));

    EXPECT_CALL(renderedTarget1, mapFromScene(scenePos)).WillRepeatedly(Return(localPos));
    EXPECT_CALL(renderedTarget2, mapFromScene(scenePos)).WillRepeatedly(Return(localPos));
    EXPECT_CALL(renderedTarget3, mapFromScene(scenePos)).WillRepeatedly(Return(localPos));

    QMouseEvent pressEvent(QEvent::MouseButtonPress, scenePos, scenePos, globalPos, Qt::LeftButton, Qt::LeftButton | Qt::RightButton, Qt::ShiftModifier, &dev);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, scenePos, scenePos, globalPos, Qt::LeftButton, Qt::LeftButton | Qt::RightButton, Qt::ShiftModifier, &dev);
    QSignalSpy pressedSpy(&handler, &MouseEventHandler::mousePressed);
    QSignalSpy releasedSpy(&handler, &MouseEventHandler::mouseReleased);

    auto checkMouseEvent = [&dev](QMouseEvent *event, QEvent::Type type) {
        ASSERT_TRUE(event);
        ASSERT_EQ(event->type(), type);
        ASSERT_EQ(event->position(), scenePos);
        ASSERT_EQ(event->scenePosition(), scenePos);
        ASSERT_EQ(event->globalPosition(), globalPos);
        ASSERT_EQ(event->button(), Qt::LeftButton);
        ASSERT_EQ(event->buttons(), Qt::LeftButton | Qt::RightButton);
        ASSERT_EQ(event->modifiers(), Qt::ShiftModifier);
        ASSERT_EQ(event->device(), &dev);
    };

    auto checkPressEvent = [checkMouseEvent](QMouseEvent *event) { checkMouseEvent(event, QEvent::MouseButtonPress); };
    auto checkReleaseEvent = [checkMouseEvent](QMouseEvent *event) { checkMouseEvent(event, QEvent::MouseButtonRelease); };

    // Send to sprite 1 (press)
    EXPECT_CALL(renderedTarget3, contains(localPos)).WillOnce(Return(false));
    EXPECT_CALL(renderedTarget1, contains(localPos)).WillOnce(Return(true));
    EXPECT_CALL(renderedTarget1, mousePressEvent(_)).WillOnce(WithArgs<0>(Invoke(checkPressEvent)));
    ASSERT_TRUE(handler.eventFilter(nullptr, &pressEvent));
    ASSERT_EQ(pressedSpy.count(), 1);
    ASSERT_EQ(releasedSpy.count(), 0);
    pressedSpy.clear();
    releasedSpy.clear();

    // Send release (should be sent to sprite 1)
    EXPECT_CALL(renderedTarget1, mouseReleaseEvent(_)).WillOnce(WithArgs<0>(Invoke(checkReleaseEvent)));
    ASSERT_TRUE(handler.eventFilter(nullptr, &releaseEvent));
    ASSERT_EQ(pressedSpy.count(), 0);
    ASSERT_EQ(releasedSpy.count(), 1);
    pressedSpy.clear();
    releasedSpy.clear();

    // Send to sprite 1 (press)
    EXPECT_CALL(renderedTarget3, contains(localPos)).WillOnce(Return(false));
    EXPECT_CALL(renderedTarget1, contains(localPos)).WillOnce(Return(true));
    EXPECT_CALL(renderedTarget1, mousePressEvent(_)).WillOnce(WithArgs<0>(Invoke(checkPressEvent)));
    ASSERT_TRUE(handler.eventFilter(nullptr, &pressEvent));
    ASSERT_EQ(pressedSpy.count(), 1);
    ASSERT_EQ(releasedSpy.count(), 0);
    pressedSpy.clear();
    releasedSpy.clear();

    // Send release (should be sent to sprite 1)
    EXPECT_CALL(renderedTarget1, mouseReleaseEvent(_)).WillOnce(WithArgs<0>(Invoke(checkReleaseEvent)));
    ASSERT_TRUE(handler.eventFilter(nullptr, &releaseEvent));
    ASSERT_EQ(pressedSpy.count(), 0);
    ASSERT_EQ(releasedSpy.count(), 1);
    pressedSpy.clear();
    releasedSpy.clear();

    // Send to sprite 1 (release)
    EXPECT_CALL(renderedTarget3, contains(localPos)).WillOnce(Return(false));
    EXPECT_CALL(renderedTarget1, contains(localPos)).WillOnce(Return(true));
    EXPECT_CALL(renderedTarget1, mouseReleaseEvent(_)).WillOnce(WithArgs<0>(Invoke(checkReleaseEvent)));
    ASSERT_TRUE(handler.eventFilter(nullptr, &releaseEvent));
    ASSERT_EQ(pressedSpy.count(), 0);
    ASSERT_EQ(releasedSpy.count(), 1);
    pressedSpy.clear();
    releasedSpy.clear();

    // Send to stage (press)
    EXPECT_CALL(renderedTarget3, contains(localPos)).WillOnce(Return(false));
    EXPECT_CALL(renderedTarget1, contains(localPos)).WillOnce(Return(false));
    EXPECT_CALL(renderedTarget2, contains(localPos)).WillOnce(Return(false));
    EXPECT_CALL(stage, mousePressEvent(_)).WillOnce(WithArgs<0>(Invoke(checkPressEvent)));
    ASSERT_TRUE(handler.eventFilter(nullptr, &pressEvent));
    ASSERT_EQ(pressedSpy.count(), 1);
    ASSERT_EQ(releasedSpy.count(), 0);
    pressedSpy.clear();
    releasedSpy.clear();

    // Send release (should be sent to stage)
    EXPECT_CALL(stage, mouseReleaseEvent(_)).WillOnce(WithArgs<0>(Invoke(checkReleaseEvent)));
    ASSERT_TRUE(handler.eventFilter(nullptr, &releaseEvent));
    ASSERT_EQ(pressedSpy.count(), 0);
    ASSERT_EQ(releasedSpy.count(), 1);
    pressedSpy.clear();
    releasedSpy.clear();
}
