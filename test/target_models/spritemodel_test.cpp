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

    auto c1 = std::make_shared<Costume>("", "", "");
    auto c2 = std::make_shared<Costume>("", "", "");
    auto c3 = std::make_shared<Costume>("", "", "");
    sprite.addCostume(c1);
    sprite.addCostume(c2);
    sprite.addCostume(c3);
    sprite.setCostumeIndex(1);

    RenderedTargetMock renderedTarget;
    QSignalSpy spy(&model, &SpriteModel::renderedTargetChanged);
    EXPECT_CALL(renderedTarget, loadCostume(c2.get()));
    model.setRenderedTarget(&renderedTarget);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(model.renderedTarget(), &renderedTarget);

    EXPECT_CALL(renderedTarget, loadCostume(c2.get()));
    model.init(&sprite);
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

TEST(SpriteModelTest, OnSizeChanged)
{
    SpriteModel model;

    Costume costume("", "", "");

    Sprite sprite;
    model.init(&sprite);
    ASSERT_EQ(model.sprite(), &sprite);

    auto c1 = std::make_shared<Costume>("", "", "");
    auto c2 = std::make_shared<Costume>("", "", "");
    auto c3 = std::make_shared<Costume>("", "", "");
    sprite.addCostume(c1);
    sprite.addCostume(c2);
    sprite.addCostume(c3);
    sprite.setCostumeIndex(1);

    RenderedTargetMock renderedTarget;
    EXPECT_CALL(renderedTarget, loadCostume(c2.get()));
    model.setRenderedTarget(&renderedTarget);

    EXPECT_CALL(renderedTarget, loadCostume(c2.get()));
    model.onSizeChanged(150);
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
