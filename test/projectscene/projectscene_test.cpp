#include <projectscene.h>
#include <enginemock.h>

#include "../common.h"

using namespace scratchcppgui;

using ::testing::Return;

TEST(ProjectScene, Engine)
{
    ProjectScene scene;
    ASSERT_EQ(scene.engine(), nullptr);

    EngineMock engine;
    scene.setEngine(&engine);
    ASSERT_EQ(scene.engine(), &engine);
}

TEST(ProjectScene, HandleMouseMove)
{
    ProjectScene scene;
    EngineMock engine;
    scene.setEngine(&engine);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(600));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(400));
    EXPECT_CALL(engine, setMouseX(-253.1));
    EXPECT_CALL(engine, setMouseY(216.7));
    scene.handleMouseMove(46.9, -16.7);
}

TEST(ProjectScene, HandleMousePress)
{
    ProjectScene scene;
    EngineMock engine;
    scene.setEngine(&engine);

    EXPECT_CALL(engine, setMousePressed(true));
    scene.handleMousePress();

    EXPECT_CALL(engine, setMousePressed(true));
    scene.handleMousePress();
}

TEST(ProjectScene, HandleMouseRelease)
{
    ProjectScene scene;
    EngineMock engine;
    scene.setEngine(&engine);

    EXPECT_CALL(engine, setMousePressed(false));
    scene.handleMouseRelease();

    EXPECT_CALL(engine, setMousePressed(false));
    scene.handleMouseRelease();
}
