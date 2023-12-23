#include <scratchcpp/keyevent.h>
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

TEST(ProjectScene, HandleKeyPressAndRelease)
{
    static const std::unordered_map<Qt::Key, KeyEvent::Type> SPECIAL_KEY_MAP = {
        { Qt::Key_Space, KeyEvent::Type::Space }, { Qt::Key_Left, KeyEvent::Type::Left },    { Qt::Key_Up, KeyEvent::Type::Up },      { Qt::Key_Right, KeyEvent::Type::Right },
        { Qt::Key_Down, KeyEvent::Type::Down },   { Qt::Key_Return, KeyEvent::Type::Enter }, { Qt::Key_Enter, KeyEvent::Type::Enter }
    };

    ProjectScene scene;
    EngineMock engine;
    scene.setEngine(&engine);

    for (const auto &[qtKey, scratchKey] : SPECIAL_KEY_MAP) {
        KeyEvent event(scratchKey);
        EXPECT_CALL(engine, setKeyState(event.name(), true));
        scene.handleKeyPress(qtKey, "test");

        EXPECT_CALL(engine, setKeyState(event.name(), false));
        scene.handleKeyRelease(qtKey, "test");
    }

    EXPECT_CALL(engine, setKeyState("a", true));
    scene.handleKeyPress(Qt::Key_A, "a");

    EXPECT_CALL(engine, setKeyState("a", false));
    scene.handleKeyRelease(Qt::Key_A, "a");

    EXPECT_CALL(engine, setKeyState("0", true));
    scene.handleKeyPress(Qt::Key_0, "0");

    EXPECT_CALL(engine, setKeyState("0", false));
    scene.handleKeyRelease(Qt::Key_0, "0");
}