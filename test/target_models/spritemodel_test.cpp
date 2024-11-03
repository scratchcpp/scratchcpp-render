#include <QtTest/QSignalSpy>
#include <scratchcpp/costume.h>
#include <scratchcpp/value.h>
#include <scratchcpp/textbubble.h>
#include <spritemodel.h>
#include <graphicseffect.h>
#include <renderedtargetmock.h>
#include <penlayermock.h>
#include <enginemock.h>

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
    cloneModel->penAttributes().color = QNanoColor(255, 0, 0);
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
    ASSERT_EQ(cloneModel->penAttributes().color, QNanoColor(255, 0, 0));
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
    Sprite sprite;
    EngineMock engine;
    sprite.setEngine(&engine);
    model.init(&sprite);

    PenLayerMock penLayer;
    model.setPenLayer(&penLayer);

    EXPECT_CALL(penLayer, drawLine).Times(0);
    EXPECT_CALL(engine, requestRedraw).Times(0);
    model.onMoved(-15.6, 54.9, 159.04, -2.5);

    EXPECT_CALL(penLayer, drawPoint);
    EXPECT_CALL(engine, requestRedraw);
    model.setPenDown(true);
    PenAttributes &attr = model.penAttributes();

    EXPECT_CALL(penLayer, drawLine(_, -15.6, 54.9, 159.04, -2.5)).WillOnce(WithArgs<0>(Invoke([&attr](const PenAttributes &attrArg) { ASSERT_EQ(&attr, &attrArg); })));
    EXPECT_CALL(engine, requestRedraw());
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

TEST(SpriteModelTest, OnGraphicsEffectChanged)
{
    SpriteModel model;
    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    GraphicsEffect effect1(ShaderManager::Effect::Brightness, "brightness");
    EXPECT_CALL(renderedTarget, setGraphicEffect(ShaderManager::Effect::Brightness, 78.4));
    model.onGraphicsEffectChanged(&effect1, 78.4);

    GraphicsEffect effect2(ShaderManager::Effect::Ghost, "ghost");
    EXPECT_CALL(renderedTarget, setGraphicEffect(ShaderManager::Effect::Ghost, 0.0));
    model.onGraphicsEffectChanged(&effect2, 0.0);
}

TEST(SpriteModelTest, OnGraphicsEffectsCleared)
{
    SpriteModel model;
    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, clearGraphicEffects());
    model.onGraphicsEffectsCleared();
}

TEST(SpriteModelTest, OnBubbleTypeChanged)
{
    SpriteModel model;
    Sprite sprite;
    model.init(&sprite);
    QSignalSpy spy(&model, &SpriteModel::bubbleTypeChanged);
    ASSERT_EQ(model.bubbleType(), TextBubbleShape::Type::Say);

    sprite.bubble()->setType(TextBubble::Type::Think);
    ASSERT_EQ(model.bubbleType(), TextBubbleShape::Type::Think);
    ASSERT_EQ(spy.count(), 1);

    sprite.bubble()->setType(TextBubble::Type::Think);
    ASSERT_EQ(model.bubbleType(), TextBubbleShape::Type::Think);
    ASSERT_EQ(spy.count(), 1);

    sprite.bubble()->setType(TextBubble::Type::Say);
    ASSERT_EQ(model.bubbleType(), TextBubbleShape::Type::Say);
    ASSERT_EQ(spy.count(), 2);

    sprite.bubble()->setType(TextBubble::Type::Say);
    ASSERT_EQ(model.bubbleType(), TextBubbleShape::Type::Say);
    ASSERT_EQ(spy.count(), 2);
}

TEST(SpriteModelTest, OnBubbleTextChanged)
{
    SpriteModel model;
    Sprite sprite;
    model.init(&sprite);
    QSignalSpy spy(&model, &SpriteModel::bubbleTextChanged);
    ASSERT_TRUE(model.bubbleText().isEmpty());

    sprite.bubble()->setText("Hello!");
    ASSERT_EQ(model.bubbleText(), "Hello!");
    ASSERT_EQ(spy.count(), 1);

    sprite.bubble()->setText("Hello!");
    ASSERT_EQ(model.bubbleText(), "Hello!");
    ASSERT_EQ(spy.count(), 1);

    sprite.bubble()->setText("test");
    ASSERT_EQ(model.bubbleText(), "test");
    ASSERT_EQ(spy.count(), 2);
}

TEST(SpriteModelTest, CostumeWidth)
{
    SpriteModel model;

    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, costumeWidth()).WillOnce(Return(15));
    ASSERT_EQ(model.costumeWidth(), 15);
}

TEST(SpriteModelTest, CostumeHeight)
{
    SpriteModel model;

    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, costumeHeight()).WillOnce(Return(10));
    ASSERT_EQ(model.costumeHeight(), 10);
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

TEST(SpriteModelTest, FastBoundingRect)
{
    SpriteModel model;

    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    Rect rect(-1, 1, 1, -1);
    EXPECT_CALL(renderedTarget, getFastBounds()).WillOnce(Return(rect));
    Rect bounds = model.fastBoundingRect();
    ASSERT_EQ(bounds.left(), rect.left());
    ASSERT_EQ(bounds.top(), rect.top());
    ASSERT_EQ(bounds.right(), rect.right());
    ASSERT_EQ(bounds.bottom(), rect.bottom());
}

TEST(SpriteModelTest, TouchingClones)
{
    SpriteModel model;

    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    Sprite clone1, clone2;
    std::vector<Sprite *> clones = { &clone1, &clone2 };
    std::vector<Sprite *> actualClones;

    EXPECT_CALL(renderedTarget, touchingClones(_)).WillOnce(WithArgs<0>(Invoke([&actualClones](const std::vector<Sprite *> &candidates) {
        actualClones = candidates;
        return false;
    })));
    ASSERT_FALSE(model.touchingClones(clones));
    ASSERT_EQ(actualClones, clones);

    EXPECT_CALL(renderedTarget, touchingClones(_)).WillOnce(WithArgs<0>(Invoke([&actualClones](const std::vector<Sprite *> &candidates) {
        actualClones = candidates;
        return true;
    })));
    ASSERT_TRUE(model.touchingClones(clones));
    ASSERT_EQ(actualClones, clones);
}

TEST(SpriteModelTest, TouchingPoint)
{
    SpriteModel model;

    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, containsScratchPoint(56.3, -179.4)).WillOnce(Return(false));
    ASSERT_FALSE(model.touchingPoint(56.3, -179.4));

    EXPECT_CALL(renderedTarget, containsScratchPoint(-20.08, 109.47)).WillOnce(Return(true));
    ASSERT_TRUE(model.touchingPoint(-20.08, 109.47));
}

TEST(SpriteModelTest, TouchingColor)
{
    SpriteModel model;

    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    Value color1 = 123, color2 = 456;
    EXPECT_CALL(renderedTarget, touchingColor(color1)).WillOnce(Return(false));
    ASSERT_FALSE(model.touchingColor(color1));

    EXPECT_CALL(renderedTarget, touchingColor(color1)).WillOnce(Return(true));
    ASSERT_TRUE(model.touchingColor(color1));

    EXPECT_CALL(renderedTarget, touchingColor(color1, color2)).WillOnce(Return(false));
    ASSERT_FALSE(model.touchingColor(color1, color2));

    EXPECT_CALL(renderedTarget, touchingColor(color1, color2)).WillOnce(Return(true));
    ASSERT_TRUE(model.touchingColor(color1, color2));
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
    EngineMock engine;
    sprite.setX(24.6);
    sprite.setY(-48.8);
    sprite.setEngine(&engine);
    model.init(&sprite);
    ASSERT_FALSE(model.penDown());
    ASSERT_FALSE(model.penState().penDown);

    PenLayerMock penLayer;
    model.setPenLayer(&penLayer);

    PenAttributes &attr = model.penAttributes();

    EXPECT_CALL(penLayer, drawPoint(_, 24.6, -48.8)).WillOnce(WithArgs<0>(Invoke([&attr](const PenAttributes &attrArg) { ASSERT_EQ(&attr, &attrArg); })));
    EXPECT_CALL(engine, requestRedraw());
    model.setPenDown(true);
    ASSERT_TRUE(model.penDown());
    ASSERT_TRUE(model.penState().penDown);

    EXPECT_CALL(penLayer, drawPoint(_, 24.6, -48.8));
    EXPECT_CALL(engine, requestRedraw());
    model.setPenDown(true);
    ASSERT_TRUE(model.penDown());
    ASSERT_TRUE(model.penState().penDown);

    EXPECT_CALL(penLayer, drawPoint).Times(0);
    EXPECT_CALL(engine, requestRedraw).Times(0);
    model.setPenDown(false);
    ASSERT_FALSE(model.penDown());
    ASSERT_FALSE(model.penState().penDown);
}

TEST(SpriteModelTest, BubbleLayer)
{
    SpriteModel model;
    Sprite sprite;
    model.init(&sprite);
    QSignalSpy spy(&model, &SpriteModel::bubbleLayerChanged);

    sprite.bubble()->setLayerOrder(5);
    ASSERT_EQ(model.bubbleLayer(), 5);
    ASSERT_EQ(spy.count(), 1);
}
