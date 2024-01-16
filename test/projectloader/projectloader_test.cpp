#include <QtTest/QSignalSpy>
#include <projectloader.h>
#include <spritemodel.h>
#include <valuemonitormodel.h>
#include <enginemock.h>
#include <renderedtargetmock.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

using ::testing::Return;

class ProjectLoaderTest : public testing::Test
{
    public:
        void load(ProjectLoader *loader, const QString &fileName)
        {
            static const std::chrono::milliseconds timeout(5000);
            auto startTime = std::chrono::steady_clock::now();
            QSignalSpy fileNameSpy(loader, &ProjectLoader::fileNameChanged);
            QSignalSpy loadStatusSpy(loader, &ProjectLoader::loadStatusChanged);
            QSignalSpy loadingFinishedSpy(loader, &ProjectLoader::loadingFinished);
            QSignalSpy engineSpy(loader, &ProjectLoader::engineChanged);
            QSignalSpy stageSpy(loader, &ProjectLoader::stageChanged);
            QSignalSpy spritesSpy(loader, &ProjectLoader::spritesChanged);
            QSignalSpy monitorsSpy(loader, &ProjectLoader::monitorsChanged);
            QSignalSpy monitorAddedSpy(loader, &ProjectLoader::monitorAdded);

            loader->setFileName(fileName);

            ASSERT_EQ(fileNameSpy.count(), 1);
            ASSERT_EQ(loadStatusSpy.count(), 1);
            ASSERT_TRUE(loadingFinishedSpy.empty());
            ASSERT_TRUE(engineSpy.empty());
            ASSERT_TRUE(stageSpy.empty());
            ASSERT_TRUE(spritesSpy.empty());
            ASSERT_TRUE(monitorsSpy.empty());
            ASSERT_TRUE(monitorAddedSpy.empty());
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
            ASSERT_EQ(monitorsSpy.count(), loader->monitorList().size());
            ASSERT_EQ(monitorAddedSpy.count(), loader->monitorList().size());
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

    const auto &monitors = loader.monitorList();
    ASSERT_EQ(monitors.size(), 7);

    ValueMonitorModel *monitorModel = dynamic_cast<ValueMonitorModel *>(monitors[0]);
    ASSERT_EQ(monitorModel->monitor(), engine->monitors().at(3).get());
    ASSERT_EQ(monitorModel->color(), QColor::fromString("#FF8C1A"));

    monitorModel = dynamic_cast<ValueMonitorModel *>(monitors[1]);
    ASSERT_EQ(monitorModel->monitor(), engine->monitors().at(4).get());
    ASSERT_EQ(monitorModel->color(), QColor::fromString("#FF8C1A"));
}

TEST_F(ProjectLoaderTest, Clones)
{
    ProjectLoader loader;
    QSignalSpy cloneCreatedSpy(&loader, &ProjectLoader::cloneCreated);
    QSignalSpy cloneDeletedSpy(&loader, &ProjectLoader::cloneDeleted);
    QSignalSpy clonesChangedSpy(&loader, &ProjectLoader::clonesChanged);
    load(&loader, "clones.sb3");
    ASSERT_TRUE(cloneCreatedSpy.empty());
    ASSERT_TRUE(cloneDeletedSpy.empty());
    ASSERT_TRUE(clonesChangedSpy.empty());

    auto engine = loader.engine();
    engine->run();
    ASSERT_EQ(cloneCreatedSpy.count(), 3);
    ASSERT_EQ(cloneDeletedSpy.count(), 0);
    ASSERT_EQ(clonesChangedSpy.count(), 3);

    const auto &sprites = loader.spriteList();
    const auto &clones = loader.cloneList();
    ASSERT_EQ(sprites.size(), 1);
    ASSERT_EQ(clones.size(), 3);
    ASSERT_EQ(clones[0]->sprite()->cloneSprite(), sprites[0]->sprite());
    ASSERT_EQ(clones[1]->sprite()->cloneSprite(), sprites[0]->sprite());
    ASSERT_EQ(clones[2]->sprite()->cloneSprite(), sprites[0]->sprite());

    RenderedTargetMock target1, target2, target3;
    clones[0]->setRenderedTarget(&target1);
    clones[1]->setRenderedTarget(&target2);
    clones[2]->setRenderedTarget(&target3);

    EXPECT_CALL(target2, deinitClone());
    clones[1]->sprite()->deleteClone();
    ASSERT_EQ(cloneCreatedSpy.count(), 3);
    ASSERT_EQ(cloneDeletedSpy.count(), 1);
    ASSERT_EQ(clonesChangedSpy.count(), 4);
    ASSERT_EQ(clones.size(), 2);
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

TEST_F(ProjectLoaderTest, TimerEvent)
{
    ProjectLoader loader;
    EngineMock engine;
    loader.setEngine(&engine);
    QTimerEvent event(0);

    EXPECT_CALL(engine, step());
    QCoreApplication::sendEvent(&loader, &event);
}

TEST_F(ProjectLoaderTest, Fps)
{
    ProjectLoader loader;
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
