#include <QtTest/QSignalSpy>
#include <scratchcpp/costume.h>
#include <spritemodel.h>
#include <renderedtargetmock.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

using ::testing::Return;

TEST(SpriteModelTest, Constructors)
{
    SpriteModel model1;
    SpriteModel model2(&model1);
    ASSERT_EQ(model2.parent(), &model1);
}

TEST(SpriteModelTest, Init)
{
    SpriteModel model;
    ASSERT_EQ(model.sprite(), nullptr);

    Sprite sprite;
    model.init(&sprite);
    ASSERT_EQ(model.sprite(), &sprite);
}

TEST(SpriteModelTest, DeInitClone)
{
    SpriteModel model;
    QSignalSpy spy(&model, &SpriteModel::cloneDeleted);
    model.deinitClone();
    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    ASSERT_EQ(args.size(), 1);
    SpriteModel *modelPtr = args.at(0).value<SpriteModel *>();
    ASSERT_EQ(modelPtr, &model);
}

TEST(SpriteModelTest, OnCloned)
{
    SpriteModel model;
    ASSERT_EQ(model.cloneRoot(), nullptr);

    Sprite clone1;
    QSignalSpy spy1(&model, &SpriteModel::cloned);
    model.onCloned(&clone1);
    ASSERT_EQ(spy1.count(), 1);
    ASSERT_EQ(model.cloneRoot(), nullptr);

    QList<QVariant> args = spy1.takeFirst();
    ASSERT_EQ(args.size(), 1);
    SpriteModel *cloneModel = args.at(0).value<SpriteModel *>();
    ASSERT_TRUE(cloneModel);
    ASSERT_EQ(cloneModel->parent(), &model);
    ASSERT_EQ(cloneModel->sprite(), &clone1);
    ASSERT_EQ(cloneModel->cloneRoot(), &model);
    spy1.clear();

    Sprite clone2;
    model.onCloned(&clone2);
    ASSERT_EQ(spy1.count(), 1);

    args = spy1.takeFirst();
    ASSERT_EQ(args.size(), 1);
    cloneModel = args.at(0).value<SpriteModel *>();
    ASSERT_TRUE(cloneModel);
    ASSERT_EQ(cloneModel->parent(), &model);
    ASSERT_EQ(cloneModel->sprite(), &clone2);
    ASSERT_EQ(cloneModel->cloneRoot(), &model);

    Sprite clone3;
    QSignalSpy spy2(cloneModel, &SpriteModel::cloned);
    cloneModel->onCloned(&clone3);
    ASSERT_EQ(spy2.count(), 1);

    args = spy2.takeFirst();
    ASSERT_EQ(args.size(), 1);
    cloneModel = args.at(0).value<SpriteModel *>();
    ASSERT_TRUE(cloneModel);
    ASSERT_EQ(cloneModel->parent(), &model);
    ASSERT_EQ(cloneModel->sprite(), &clone3);
    ASSERT_EQ(cloneModel->cloneRoot(), &model);
}

TEST(SpriteModelTest, OnCostumeChanged)
{
    SpriteModel model;

    Costume costume("", "", "");

    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, updateCostume(&costume));
    model.onCostumeChanged(&costume);
}

TEST(SpriteModelTest, OnVisibleChanged)
{
    SpriteModel model;
    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, updateVisibility(true));
    model.onVisibleChanged(true);

    EXPECT_CALL(renderedTarget, updateVisibility(false));
    model.onVisibleChanged(false);
}

TEST(SpriteModelTest, OnXChanged)
{
    SpriteModel model;
    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, updateX(32.4));
    model.onXChanged(32.4);
}

TEST(SpriteModelTest, OnYChanged)
{
    SpriteModel model;
    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, updateY(-46.1));
    model.onYChanged(-46.1);
}

TEST(SpriteModelTest, OnSizeChanged)
{
    SpriteModel model;
    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, updateSize(65.2));
    model.onSizeChanged(65.2);
}

TEST(SpriteModelTest, OnDirectionChanged)
{
    SpriteModel model;
    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, updateDirection(-5.4));
    model.onDirectionChanged(-5.4);
}

TEST(SpriteModelTest, OnRotationStyleChanged)
{
    SpriteModel model;
    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, updateRotationStyle(Sprite::RotationStyle::AllAround));
    model.onRotationStyleChanged(Sprite::RotationStyle::AllAround);

    EXPECT_CALL(renderedTarget, updateRotationStyle(Sprite::RotationStyle::LeftRight));
    model.onRotationStyleChanged(Sprite::RotationStyle::LeftRight);

    EXPECT_CALL(renderedTarget, updateRotationStyle(Sprite::RotationStyle::DoNotRotate));
    model.onRotationStyleChanged(Sprite::RotationStyle::DoNotRotate);
}

TEST(SpriteModelTest, OnLayerOrderChanged)
{
    SpriteModel model;
    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, updateLayerOrder(7));
    model.onLayerOrderChanged(7);
}

TEST(SpriteModelTest, BoundingRect)
{
    SpriteModel model;

    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    Rect rect(-1, 1, 1, -1);
    EXPECT_CALL(renderedTarget, getBounds()).WillOnce(Return(rect));
    Rect bounds = model.boundingRect();
    ASSERT_EQ(bounds.left(), rect.left());
    ASSERT_EQ(bounds.top(), rect.top());
    ASSERT_EQ(bounds.right(), rect.right());
    ASSERT_EQ(bounds.bottom(), rect.bottom());
}

TEST(SpriteModelTest, RenderedTarget)
{
    SpriteModel model;
    ASSERT_EQ(model.renderedTarget(), nullptr);

    RenderedTargetMock renderedTarget;
    QSignalSpy spy(&model, &SpriteModel::renderedTargetChanged);
    model.setRenderedTarget(&renderedTarget);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(model.renderedTarget(), &renderedTarget);
}
