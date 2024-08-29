#include <QtTest/QSignalSpy>
#include <scratchcpp/monitor.h>
#include <scratchcpp/virtualmachine.h>
#include <scratchcpp/block.h>
#include <scratchcpp/comment.h>
#include <valuemonitormodel.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

TEST(ValueMonitorModelTest, Constructors)
{
    {
        ValueMonitorModel model1;
        ValueMonitorModel model2(&model1);
        ASSERT_EQ(model2.parent(), &model1);
    }

    {
        ValueMonitorModel model1;
        ValueMonitorModel model2(nullptr, &model1);
        ASSERT_EQ(model2.parent(), &model1);
    }
}

TEST(ValueMonitorModelTest, OnValueChanged)
{
    ValueMonitorModel model;
    QSignalSpy spy(&model, &ValueMonitorModel::valueChanged);
    VirtualMachine vm;

    vm.addReturnValue(5.4);
    model.onValueChanged(&vm);
    ASSERT_EQ(model.value(), "5.4");
    ASSERT_EQ(spy.count(), 1);

    vm.reset();
    vm.addReturnValue("test");
    model.onValueChanged(&vm);
    ASSERT_EQ(model.value(), "test");
    ASSERT_EQ(spy.count(), 2);
}

TEST(ValueMonitorModelTest, Value)
{
    ValueMonitorModel model;
    QSignalSpy spy(&model, &ValueMonitorModel::valueChanged);
    ASSERT_TRUE(model.value().isEmpty());

    model.setValue("hello");
    ASSERT_EQ(model.value(), "hello");
    ASSERT_EQ(spy.count(), 1);

    model.setValue("world");
    ASSERT_EQ(model.value(), "world");
    ASSERT_EQ(spy.count(), 2);

    Monitor monitor("", "");
    monitor.setValueChangeFunction([](Block *block, const Value &newValue) {
        auto comment = std::make_shared<Comment>("");
        comment->setText("it works");
        block->setComment(comment);
    });
    model.init(&monitor);

    model.setValue("test");
    ASSERT_EQ(model.value(), "world");
    ASSERT_TRUE(monitor.block()->comment());
    ASSERT_EQ(monitor.block()->comment()->text(), "it works");
    ASSERT_EQ(spy.count(), 2);
}

TEST(ValueMonitorModelTest, Type)
{
    ValueMonitorModel model;
    ASSERT_EQ(model.type(), MonitorModel::Type::Value);
}

TEST(ValueMonitorModelTest, Mode)
{
    ValueMonitorModel model;
    Monitor monitor("", "");
    monitor.setMode(Monitor::Mode::Default);
    model.init(&monitor);
    ASSERT_EQ(model.mode(), ValueMonitorModel::Mode::Default);

    monitor.setMode(Monitor::Mode::Large);
    ASSERT_EQ(model.mode(), ValueMonitorModel::Mode::Large);

    monitor.setMode(Monitor::Mode::Slider);
    ASSERT_EQ(model.mode(), ValueMonitorModel::Mode::Slider);
}

TEST(ValueMonitorModelTest, SliderMin)
{
    ValueMonitorModel model;
    Monitor monitor("", "");
    monitor.setSliderMin(-0.5);
    model.init(&monitor);
    ASSERT_EQ(model.sliderMin(), -0.5);

    monitor.setSliderMin(2.65);
    ASSERT_EQ(model.sliderMin(), 2.65);
}

TEST(ValueMonitorModelTest, SliderMax)
{
    ValueMonitorModel model;
    Monitor monitor("", "");
    monitor.setSliderMax(-0.5);
    model.init(&monitor);
    ASSERT_EQ(model.sliderMax(), -0.5);

    monitor.setSliderMax(2.65);
    ASSERT_EQ(model.sliderMax(), 2.65);
}

TEST(ValueMonitorModelTest, Discrete)
{
    ValueMonitorModel model;
    Monitor monitor("", "");
    monitor.setDiscrete(true);
    model.init(&monitor);
    ASSERT_TRUE(model.discrete());

    monitor.setDiscrete(false);
    ASSERT_FALSE(model.discrete());

    monitor.setDiscrete(false);
    ASSERT_FALSE(model.discrete());
}
