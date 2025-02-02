#include <QtTest/QSignalSpy>
#include <scratchcpp/costume.h>
#include <scratchcpp/value.h>
#include <scratchcpp/textbubble.h>
#include <targetmodel.h>
#include <graphicseffect.h>
#include <renderedtargetmock.h>
#include <penlayermock.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

using ::testing::Return;
using ::testing::WithArgs;
using ::testing::Invoke;
using ::testing::_;

TEST(TargetModelTest, Constructors)
{
    QObject parent;
    TargetModel model(&parent);
    ASSERT_EQ(model.parent(), &parent);
}

TEST(TargetModelTest, RenderedTarget)
{
    TargetModel model;
    ASSERT_EQ(model.renderedTarget(), nullptr);

    RenderedTargetMock renderedTarget;
    QSignalSpy spy(&model, &TargetModel::renderedTargetChanged);
    model.setRenderedTarget(&renderedTarget);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(model.renderedTarget(), &renderedTarget);
}

TEST(TargetModelTest, PenLayer)
{
    TargetModel model;
    ASSERT_EQ(model.penLayer(), nullptr);

    PenLayerMock penLayer;
    QSignalSpy spy(&model, &TargetModel::penLayerChanged);
    model.setPenLayer(&penLayer);
    ASSERT_EQ(spy.count(), 1);
    ASSERT_EQ(model.penLayer(), &penLayer);
}
