#include <QtTest/QSignalSpy>
#include <scratchcpp/costume.h>
#include <spritemodel.h>
#include <renderedtargetmock.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

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

TEST(SpriteModelTest, OnCostumeChanged)
{
    SpriteModel model;

    Costume costume("", "", "");

    RenderedTargetMock renderedTarget;
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, loadCostume(&costume));
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
