#include <QtTest/QSignalSpy>
#include <projectloader.h>
#include <spritemodel.h>
#include <enginemock.h>

#include "../common.h"

using namespace scratchcppgui;
using namespace libscratchcpp;

using ::testing::Return;

class ProjectLoaderTest : public testing::Test
{
    public:
        void load(ProjectLoader *loader, const QString &fileName)
        {
            static const std::chrono::milliseconds timeout(5000);
            auto startTime = std::chrono::steady_clock::now();
            loader->setEventLoopEnabled(false);
            QSignalSpy fileNameSpy(loader, &ProjectLoader::fileNameChanged);
            QSignalSpy loadStatusSpy(loader, &ProjectLoader::loadStatusChanged);
            QSignalSpy loadingFinishedSpy(loader, &ProjectLoader::loadingFinished);
            QSignalSpy engineSpy(loader, &ProjectLoader::engineChanged);
            QSignalSpy stageSpy(loader, &ProjectLoader::stageChanged);
            QSignalSpy spritesSpy(loader, &ProjectLoader::spritesChanged);

            loader->setFileName(fileName);

            ASSERT_EQ(fileNameSpy.count(), 1);
            ASSERT_EQ(loadStatusSpy.count(), 1);
            ASSERT_TRUE(loadingFinishedSpy.empty());
            ASSERT_TRUE(engineSpy.empty());
            ASSERT_TRUE(stageSpy.empty());
            ASSERT_TRUE(spritesSpy.empty());
            ASSERT_EQ(loader->fileName(), fileName);
            ASSERT_FALSE(loader->loadStatus());

            while (!loader->loadStatus())
                ASSERT_LE(std::chrono::steady_clock::now(), startTime + timeout);

            ASSERT_EQ(loader->fileName(), fileName);
            ASSERT_EQ(fileNameSpy.count(), 1);
            ASSERT_EQ(loadStatusSpy.count(), 2);
            ASSERT_EQ(loadingFinishedSpy.count(), 1);
            ASSERT_EQ(engineSpy.count(), 1);
            ASSERT_EQ(stageSpy.count(), 1);
            ASSERT_EQ(spritesSpy.count(), 1);
        }
};

TEST_F(ProjectLoaderTest, Constructors)
{
    ProjectLoader loader1;
    ProjectLoader loader2(&loader1);
    ASSERT_EQ(loader2.parent(), &loader1);
}

TEST_F(ProjectLoaderTest, Load)
{
    ProjectLoader loader;
    ASSERT_TRUE(loader.fileName().isEmpty());
    ASSERT_FALSE(loader.loadStatus());
    ASSERT_TRUE(loader.stage());

    load(&loader, "load_test.sb3");

    auto engine = loader.engine();
    ASSERT_EQ(loader.engine()->targets().size(), 3);
    ASSERT_EQ(loader.stage()->stage(), engine->stage());
    const auto &sprites = loader.spriteList();
    ASSERT_EQ(sprites.size(), 2);
    ASSERT_EQ(sprites[0]->sprite(), engine->targetAt(1));
    ASSERT_EQ(sprites[1]->sprite(), engine->targetAt(2));
}

TEST_F(ProjectLoaderTest, StartStop)
{
    ProjectLoader loader;
    load(&loader, "empty_project.sb3");

    EngineMock engine;
    loader.setEngine(&engine);

    EXPECT_CALL(engine, start());
    loader.start();

    EXPECT_CALL(engine, stop());
    loader.stop();
}

TEST_F(ProjectLoaderTest, Fps)
{
    ProjectLoader loader;
    loader.setEventLoopEnabled(false);
    EngineMock engine;
    loader.setEngine(&engine);
    ASSERT_EQ(loader.fps(), 30);

    EXPECT_CALL(engine, setFps(34.8));
    EXPECT_CALL(engine, fps()).WillOnce(Return(34.8));
    QSignalSpy spy(&loader, SIGNAL(fpsChanged()));
    loader.setFps(34.8);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(loader.fps(), 34.8);

    spy.clear();
    loader.setEngine(nullptr);
    loader.setFps(57.6);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(loader.fps(), 57.6);
}

TEST_F(ProjectLoaderTest, TurboMode)
{
    ProjectLoader loader;
    loader.setEventLoopEnabled(false);
    EngineMock engine;
    loader.setEngine(&engine);
    ASSERT_FALSE(loader.turboMode());

    EXPECT_CALL(engine, setTurboModeEnabled(true));
    QSignalSpy spy(&loader, SIGNAL(turboModeChanged()));
    loader.setTurboMode(true);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_TRUE(loader.turboMode());

    EXPECT_CALL(engine, setTurboModeEnabled(false));
    spy.clear();
    loader.setTurboMode(false);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_FALSE(loader.turboMode());
}

TEST_F(ProjectLoaderTest, StageWidth)
{
    ProjectLoader loader;
    loader.setEventLoopEnabled(false);
    EngineMock engine;
    loader.setEngine(&engine);
    ASSERT_EQ(loader.stageWidth(), 480);

    EXPECT_CALL(engine, setStageWidth(567));
    QSignalSpy spy(&loader, SIGNAL(stageWidthChanged()));
    loader.setStageWidth(567);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(loader.stageWidth(), 567);
}

TEST_F(ProjectLoaderTest, StageHeight)
{
    ProjectLoader loader;
    loader.setEventLoopEnabled(false);
    EngineMock engine;
    loader.setEngine(&engine);
    ASSERT_EQ(loader.stageHeight(), 360);

    EXPECT_CALL(engine, setStageHeight(462));
    QSignalSpy spy(&loader, SIGNAL(stageHeightChanged()));
    loader.setStageHeight(462);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(loader.stageHeight(), 462);
}

TEST_F(ProjectLoaderTest, CloneLimit)
{
    ProjectLoader loader;
    loader.setEventLoopEnabled(false);
    EngineMock engine;
    loader.setEngine(&engine);
    ASSERT_EQ(loader.cloneLimit(), 300);

    EXPECT_CALL(engine, setCloneLimit(379));
    EXPECT_CALL(engine, cloneLimit()).WillOnce(Return(379));
    QSignalSpy spy(&loader, SIGNAL(cloneLimitChanged()));
    loader.setCloneLimit(379);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(loader.cloneLimit(), 379);

    spy.clear();
    loader.setEngine(nullptr);
    loader.setCloneLimit(291);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(loader.cloneLimit(), 291);
}

TEST_F(ProjectLoaderTest, SpriteFencing)
{
    ProjectLoader loader;
    loader.setEventLoopEnabled(false);
    EngineMock engine;
    loader.setEngine(&engine);
    ASSERT_TRUE(loader.spriteFencing());

    EXPECT_CALL(engine, setSpriteFencingEnabled(false));
    QSignalSpy spy(&loader, SIGNAL(spriteFencingChanged()));
    loader.setSpriteFencing(false);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_FALSE(loader.spriteFencing());

    EXPECT_CALL(engine, setSpriteFencingEnabled(true));
    spy.clear();
    loader.setSpriteFencing(true);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_TRUE(loader.spriteFencing());
}

TEST_F(ProjectLoaderTest, EventLoopEnabled)
{
    ProjectLoader loader;
    EngineMock engine;
    loader.setEngine(&engine);
    ASSERT_TRUE(loader.eventLoopEnabled());

    EXPECT_CALL(engine, stopEventLoop());
    QSignalSpy spy(&loader, SIGNAL(eventLoopEnabledChanged()));
    loader.setEventLoopEnabled(false);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_FALSE(loader.eventLoopEnabled());

    EXPECT_CALL(engine, runEventLoop());
    spy.clear();
    loader.setEventLoopEnabled(true);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_TRUE(loader.eventLoopEnabled());

    EXPECT_CALL(engine, stopEventLoop());
    spy.clear();
    loader.setEventLoopEnabled(false);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_FALSE(loader.eventLoopEnabled());
}
