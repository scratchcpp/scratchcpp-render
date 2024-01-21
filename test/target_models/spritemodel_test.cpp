#include <QtTest/QSignalSpy>
#include <scratchcpp/costume.h>
#include <spritemodel.h>
#include <renderedtargetmock.h>
#include <penlayermock.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

using ::testing::Return;
using ::testing::WithArgs;
using ::testing::Invoke;
using ::testing::_;

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
    ASSERT_FALSE(cloneModel->penDown());

    Sprite clone3;
    QSignalSpy spy2(cloneModel, &SpriteModel::cloned);
    PenLayerMock penLayer;
    cloneModel->setPenLayer(&penLayer);
    cloneModel->penAttributes().color = QColor(255, 0, 0);
    cloneModel->penAttributes().diameter = 20.3;
    EXPECT_CALL(penLayer, drawPoint);
    cloneModel->setPenDown(true);
    cloneModel->onCloned(&clone3);
    ASSERT_EQ(spy2.count(), 1);

    args = spy2.takeFirst();
    ASSERT_EQ(args.size(), 1);
    cloneModel = args.at(0).value<SpriteModel *>();
    ASSERT_TRUE(cloneModel);
    ASSERT_EQ(cloneModel->parent(), &model);
    ASSERT_EQ(cloneModel->sprite(), &clone3);
    ASSERT_EQ(cloneModel->cloneRoot(), &model);
    ASSERT_EQ(cloneModel->penLayer(), &penLayer);
    ASSERT_EQ(cloneModel->penAttributes().color, QColor(255, 0, 0));
    ASSERT_EQ(cloneModel->penAttributes().diameter, 20.3);
    ASSERT_TRUE(cloneModel->penDown());
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

TEST(SpriteModelTest, OnMoved)
{
    SpriteModel model;

    PenLayerMock penLayer;
    model.setPenLayer(&penLayer);

    EXPECT_CALL(penLayer, drawLine).Times(0);
    model.onMoved(-15.6, 54.9, 159.04, -2.5);

    model.setPenDown(true);
    PenAttributes &attr = model.penAttributes();

    EXPECT_CALL(penLayer, drawLine(_, -15.6, 54.9, 159.04, -2.5)).WillOnce(WithArgs<0>(Invoke([&attr](const PenAttributes &attrArg) { ASSERT_EQ(&attr, &attrArg); })));
    model.onMoved(-15.6, 54.9, 159.04, -2.5);
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

TEST(SpriteModelTest, PenLayer)
{
    SpriteModel model;
    ASSERT_EQ(model.penLayer(), nullptr);

    PenLayerMock penLayer;
    QSignalSpy spy(&model, &SpriteModel::penLayerChanged);
    model.setPenLayer(&penLayer);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(model.penLayer(), &penLayer);
}

TEST(SpriteModelTest, PenDown)
{
    SpriteModel model;
    Sprite sprite;
    sprite.setX(24.6);
    sprite.setY(-48.8);
    model.init(&sprite);
    ASSERT_FALSE(model.penDown());

    PenLayerMock penLayer;
    model.setPenLayer(&penLayer);

    PenAttributes &attr = model.penAttributes();

    EXPECT_CALL(penLayer, drawPoint(_, 24.6, -48.8)).WillOnce(WithArgs<0>(Invoke([&attr](const PenAttributes &attrArg) { ASSERT_EQ(&attr, &attrArg); })));
    model.setPenDown(true);
    ASSERT_TRUE(model.penDown());

    EXPECT_CALL(penLayer, drawPoint).Times(0);
    model.setPenDown(true);
    ASSERT_TRUE(model.penDown());

    EXPECT_CALL(penLayer, drawPoint).Times(0);
    model.setPenDown(false);
    ASSERT_FALSE(model.penDown());
}
