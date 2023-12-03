#include <QtTest/QSignalSpy>
#include <renderedtarget.h>
#include <stagemodel.h>
#include <spritemodel.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/costume.h>
#include <enginemock.h>

#include "../common.h"

using namespace scratchcppgui;
using namespace libscratchcpp;

using ::testing::Return;

TEST(RenderedTargetTest, Constructors)
{
    RenderedTarget target1;
    RenderedTarget target2(&target1);
    ASSERT_EQ(target2.parent(), &target1);
    ASSERT_EQ(target2.parentItem(), &target1);
}

TEST(RenderedTargetTest, LoadAndUpdateProperties)
{
    RenderedTarget target;
    QSignalSpy mirrorHorizontallySpy(&target, &RenderedTarget::mirrorHorizontallyChanged);

    // Stage
    Stage stage;
    StageModel stageModel;
    stage.setInterface(&stageModel);
    target.setStageModel(&stageModel);
    Costume costume("", "", "");
    costume.setRotationCenterX(-23);
    costume.setRotationCenterY(72);
    EngineMock engine;
    target.loadCostume(&costume);
    target.setEngine(&engine);
    target.setCostumeWidth(102.3);
    target.setCostumeHeight(80.7);

    target.setWidth(14.3);
    target.setHeight(5.8);
    target.setX(64.5);
    target.setY(-43.7);
    target.setZ(2.5);
    target.setRotation(-78.05);
    target.setTransformOriginPoint(QPointF(3.4, 9.7));

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(544));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(249));
    target.loadProperties();
    ASSERT_EQ(target.width(), 14.3);
    ASSERT_EQ(target.height(), 5.8);
    ASSERT_EQ(target.x(), 64.5);
    ASSERT_EQ(target.y(), -43.7);
    ASSERT_EQ(target.z(), 2.5);
    ASSERT_EQ(target.rotation(), -78.05);
    ASSERT_EQ(target.transformOriginPoint(), QPointF(3.4, 9.7));

    target.updateProperties();
    ASSERT_EQ(target.width(), 102.3);
    ASSERT_EQ(target.height(), 80.7);
    ASSERT_EQ(target.x(), 283.5);
    ASSERT_EQ(target.y(), 88.5);
    ASSERT_EQ(target.z(), 0);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(target.transformOriginPoint(), QPointF(-11.5, 36));

    target.setStageModel(nullptr);
    ASSERT_TRUE(mirrorHorizontallySpy.empty());

    // Sprite
    Sprite sprite;
    sprite.setVisible(true);
    sprite.setRotationStyle(Sprite::RotationStyle::AllAround);
    sprite.setDirection(-67.16);
    sprite.setSize(143.98);
    sprite.setX(-67.94);
    sprite.setY(121.76);
    sprite.setLayerOrder(3);
    SpriteModel spriteModel;
    sprite.setInterface(&spriteModel);
    target.setSpriteModel(&spriteModel);

    target.setWidth(14.3);
    target.setHeight(5.8);
    target.setX(64.5);
    target.setY(-43.7);
    target.setZ(2.5);
    target.setRotation(-78.05);
    target.setTransformOriginPoint(QPointF(3.4, 9.7));

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(544));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(249));
    target.loadProperties();
    ASSERT_EQ(target.width(), 14.3);
    ASSERT_EQ(target.height(), 5.8);
    ASSERT_EQ(target.x(), 64.5);
    ASSERT_EQ(target.y(), -43.7);
    ASSERT_EQ(target.z(), 2.5);
    ASSERT_EQ(target.rotation(), -78.05);
    ASSERT_EQ(target.transformOriginPoint(), QPointF(3.4, 9.7));

    target.updateProperties();
    ASSERT_EQ(target.width(), 102.3);
    ASSERT_EQ(target.height(), 80.7);
    ASSERT_EQ(std::round(target.x() * 100) / 100, 220.62);
    ASSERT_EQ(std::round(target.y() * 100) / 100, -49.09);
    ASSERT_EQ(target.z(), 3);
    ASSERT_EQ(target.rotation(), -157.16);
    ASSERT_EQ(target.transformOriginPoint(), QPointF(-16.5577, 51.8328));
    ASSERT_TRUE(mirrorHorizontallySpy.empty());

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(544));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(249));
    sprite.setRotationStyle(Sprite::RotationStyle::LeftRight);
    target.loadProperties();
    ASSERT_EQ(target.mirrorHorizontally(), false);
    ASSERT_EQ(target.rotation(), -157.16);
    ASSERT_TRUE(mirrorHorizontallySpy.empty());

    target.updateProperties();
    ASSERT_EQ(target.mirrorHorizontally(), true);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(mirrorHorizontallySpy.count(), 1);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(544));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(249));
    sprite.setDirection(134.89);
    target.loadProperties();
    ASSERT_EQ(target.mirrorHorizontally(), true);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(mirrorHorizontallySpy.count(), 1);

    target.updateProperties();
    ASSERT_EQ(target.mirrorHorizontally(), false);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(mirrorHorizontallySpy.count(), 2);

    EXPECT_CALL(engine, stageWidth()).WillOnce(Return(544));
    EXPECT_CALL(engine, stageHeight()).WillOnce(Return(249));
    sprite.setRotationStyle(Sprite::RotationStyle::DoNotRotate);
    target.loadProperties();
    ASSERT_EQ(target.mirrorHorizontally(), false);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(mirrorHorizontallySpy.count(), 2);

    target.updateProperties();
    ASSERT_EQ(target.mirrorHorizontally(), false);
    ASSERT_EQ(target.rotation(), 0);
    ASSERT_EQ(mirrorHorizontallySpy.count(), 2);
}

TEST(RenderedTargetTest, LoadJpegCostume)
{
    std::string str = readFileStr("image.jpg");
    Costume costume("", "", "");
    costume.setData(str.size(), static_cast<void *>(const_cast<char *>(str.c_str())));
    costume.setBitmapResolution(3);
    costume.setId("abc");

    RenderedTarget target;
    ASSERT_EQ(target.costumeWidth(), 0);
    ASSERT_EQ(target.costumeHeight(), 0);

    target.loadCostume(&costume);
    ASSERT_EQ(target.costumeWidth(), 4 / 3.0);
    ASSERT_EQ(target.costumeHeight(), 2);
    ASSERT_FALSE(target.bitmapBuffer()->isOpen());
    target.bitmapBuffer()->open(QBuffer::ReadOnly);
    ASSERT_EQ(target.bitmapBuffer()->readAll().toStdString(), str);
    ASSERT_EQ(target.bitmapUniqueKey().toStdString(), costume.id());
    ASSERT_EQ(target.svgBitmap(), nullptr);
}

TEST(RenderedTargetTest, LoadPngCostume)
{
    std::string str = readFileStr("image.png");
    Costume costume("", "", "");
    costume.setData(str.size(), static_cast<void *>(const_cast<char *>(str.c_str())));
    costume.setBitmapResolution(3);
    costume.setId("abc");

    RenderedTarget target;
    ASSERT_EQ(target.costumeWidth(), 0);
    ASSERT_EQ(target.costumeHeight(), 0);

    target.loadCostume(&costume);
    ASSERT_EQ(target.costumeWidth(), 4 / 3.0);
    ASSERT_EQ(target.costumeHeight(), 2);
    ASSERT_FALSE(target.bitmapBuffer()->isOpen());
    target.bitmapBuffer()->open(QBuffer::ReadOnly);
    ASSERT_EQ(target.bitmapBuffer()->readAll().toStdString(), str);
    ASSERT_EQ(target.bitmapUniqueKey().toStdString(), costume.id());
    ASSERT_EQ(target.svgBitmap(), nullptr);
}

TEST(RenderedTargetTest, Engine)
{
    RenderedTarget target;
    ASSERT_EQ(target.engine(), nullptr);

    EngineMock engine;
    target.setEngine(&engine);
    ASSERT_EQ(target.engine(), &engine);
}

TEST(RenderedTargetTest, StageModel)
{
    RenderedTarget target;
    ASSERT_EQ(target.stageModel(), nullptr);

    StageModel model;
    target.setStageModel(&model);
    ASSERT_EQ(target.stageModel(), &model);
}

TEST(RenderedTargetTest, SpriteModel)
{
    RenderedTarget target;
    ASSERT_EQ(target.spriteModel(), nullptr);

    SpriteModel model;
    target.setSpriteModel(&model);
    ASSERT_EQ(target.spriteModel(), &model);
}

TEST(RenderedTargetTest, ScratchTarget)
{
    RenderedTarget target;
    ASSERT_EQ(target.scratchTarget(), nullptr);

    StageModel stageModel;
    Stage stage;
    stageModel.init(&stage);
    target.setStageModel(&stageModel);
    ASSERT_EQ(target.scratchTarget(), &stage);

    target.setStageModel(nullptr);
    SpriteModel spriteModel;
    Sprite sprite;
    spriteModel.init(&sprite);
    target.setSpriteModel(&spriteModel);
    ASSERT_EQ(target.scratchTarget(), &sprite);
}

TEST(RenderedTargetTest, CostumeWidth)
{
    RenderedTarget target;
    ASSERT_EQ(target.costumeWidth(), 0);

    target.setCostumeWidth(64.15);
    ASSERT_EQ(target.costumeWidth(), 64.15);
}

TEST(RenderedTargetTest, CostumeHeight)
{
    RenderedTarget target;
    ASSERT_EQ(target.costumeHeight(), 0);

    target.setCostumeHeight(46.48);
    ASSERT_EQ(target.costumeHeight(), 46.48);
}
