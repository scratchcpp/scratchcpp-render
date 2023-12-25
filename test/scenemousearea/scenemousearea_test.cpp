#include <QtTest/QSignalSpy>
#include <scenemousearea.h>
#include <renderedtargetmock.h>

#include "../common.h"

using namespace scratchcppgui;

TEST(SceneMouseAreaTest, Constructors)
{
    SceneMouseArea area1;
    SceneMouseArea area2(&area1);
    ASSERT_EQ(area2.parent(), &area1);
    ASSERT_EQ(area2.parentItem(), &area1);
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

TEST(SceneMouseAreaTest, SpriteRepeater)
{
    SceneMouseArea mouseArea;
    QSignalSpy spy(&mouseArea, &SceneMouseArea::spriteRepeaterChanged);
    ASSERT_EQ(mouseArea.spriteRepeater(), nullptr);

    QQuickItem item;
    mouseArea.setSpriteRepeater(&item);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(mouseArea.spriteRepeater(), &item);
}
