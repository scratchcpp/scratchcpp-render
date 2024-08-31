#include <QtTest/QSignalSpy>
#include <scenemousearea.h>
#include <projectloader.h>
#include <renderedtargetmock.h>

#include "../common.h"

using namespace scratchcpprender;

using ::testing::NiceMock;

TEST(SceneMouseAreaTest, Constructors)
{
    SceneMouseArea area1;
    SceneMouseArea area2(&area1);
    ASSERT_EQ(area2.parent(), &area1);
    ASSERT_EQ(area2.parentItem(), &area1);
}

TEST(SceneMouseAreaTest, Events)
{
    QPointingDevice dev;
    SceneMouseArea mouseArea;
    ProjectLoader loader;
    mouseArea.setProjectLoader(&loader);
    NiceMock<RenderedTargetMock> stage;
    mouseArea.setStage(&stage);

    // mouseMoved
    {
        QHoverEvent event(QEvent::HoverMove, {}, {}, {}, Qt::NoModifier, &dev);
        QSignalSpy spy(&mouseArea, &SceneMouseArea::mouseMoved);
        QCoreApplication::sendEvent(&mouseArea, &event);
        ASSERT_EQ(spy.count(), 1);
    }

    // mousePressed
    {
        QMouseEvent event(QEvent::MouseButtonPress, {}, {}, {}, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier, &dev);
        QSignalSpy spy(&mouseArea, &SceneMouseArea::mousePressed);
        QCoreApplication::sendEvent(&mouseArea, &event);
        ASSERT_EQ(spy.count(), 1);
    }

    // mouseReleased
    {
        QMouseEvent event(QEvent::MouseButtonRelease, {}, {}, {}, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier, &dev);
        QSignalSpy spy(&mouseArea, &SceneMouseArea::mouseReleased);
        QCoreApplication::sendEvent(&mouseArea, &event);
        ASSERT_EQ(spy.count(), 1);
    }

    // mouseWheelUp
    {
        QWheelEvent event(QPointF(), QPointF(), QPoint(2, 3), QPoint(10, 15), Qt::LeftButton, Qt::NoModifier, Qt::NoScrollPhase, false);
        QSignalSpy spy(&mouseArea, &SceneMouseArea::mouseWheelUp);
        QCoreApplication::sendEvent(&mouseArea, &event);
        ASSERT_EQ(spy.count(), 1);
    }

    // mouseWheelDown
    {
        QWheelEvent event(QPointF(), QPointF(), QPoint(2, 3), QPoint(10, -15), Qt::LeftButton, Qt::NoModifier, Qt::NoScrollPhase, false);
        QSignalSpy spy(&mouseArea, &SceneMouseArea::mouseWheelDown);
        QCoreApplication::sendEvent(&mouseArea, &event);
        ASSERT_EQ(spy.count(), 1);
    }
}

TEST(SceneMouseAreaTest, Stage)
{
    SceneMouseArea mouseArea;
    QSignalSpy spy(&mouseArea, &SceneMouseArea::stageChanged);
    ASSERT_EQ(mouseArea.stage(), nullptr);

    RenderedTargetMock stage;
    mouseArea.setStage(&stage);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(mouseArea.stage(), &stage);
}

TEST(SceneMouseAreaTest, ProjectLoader)
{
    SceneMouseArea mouseArea;
    QSignalSpy spy(&mouseArea, &SceneMouseArea::projectLoaderChanged);
    ASSERT_EQ(mouseArea.projectLoader(), nullptr);

    ProjectLoader loader;
    mouseArea.setProjectLoader(&loader);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(mouseArea.projectLoader(), &loader);
}

TEST(SceneMouseAreaTest, DraggedSprite)
{
    SceneMouseArea mouseArea;
    ASSERT_EQ(mouseArea.draggedSprite(), nullptr);

    RenderedTargetMock sprite;
    mouseArea.setDraggedSprite(&sprite);
    ASSERT_EQ(mouseArea.draggedSprite(), &sprite);
}
