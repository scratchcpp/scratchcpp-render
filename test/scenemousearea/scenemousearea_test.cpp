#include <QtTest/QSignalSpy>
#include <scenemousearea.h>
#include <projectloader.h>
#include <renderedtargetmock.h>

#include "../common.h"

using namespace scratchcpprender;

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
