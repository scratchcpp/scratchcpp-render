#include <QtTest/QSignalSpy>
#include <scratchcpp/monitor.h>
#include <scratchcpp/sprite.h>
#include <monitormodel.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

TEST(MonitorModelTest, Constructors)
{
    MonitorModel model1;
    MonitorModel model2(&model1);
    ASSERT_EQ(model2.parent(), &model1);
}

TEST(MonitorModelTest, Init)
{
    MonitorModel model;
    ASSERT_EQ(model.monitor(), nullptr);

    Monitor monitor("", "");
    model.init(&monitor);
    ASSERT_EQ(model.monitor(), &monitor);
}

TEST(MonitorModelTest, OnVisibleChanged)
{
    MonitorModel model;
    QSignalSpy spy(&model, &MonitorModel::visibleChanged);

    model.onVisibleChanged(false);
    ASSERT_EQ(spy.count(), 1);

    model.onVisibleChanged(true);
    ASSERT_EQ(spy.count(), 2);
}

TEST(MonitorModelTest, Name)
{
    MonitorModel model;
    Monitor monitor("", "");
    // TODO: Use monitor.setName()
    const_cast<std::string *>(&monitor.name())->assign("days since 2000");
    model.init(&monitor);
    ASSERT_EQ(model.name().toStdString(), monitor.name());

    Sprite sprite;
    sprite.setName("Sprite2");
    monitor.setSprite(&sprite);
    // TODO: Use monitor.setName()
    const_cast<std::string *>(&monitor.name())->assign("x position");
    ASSERT_EQ(model.name().toStdString(), sprite.name() + ": " + monitor.name());
}

TEST(MonitorModelTest, Visible)
{
    MonitorModel model;
    Monitor monitor("", "");
    monitor.setVisible(true);
    model.init(&monitor);
    ASSERT_TRUE(model.visible());

    monitor.setVisible(false);
    ASSERT_FALSE(model.visible());

    monitor.setVisible(false);
    ASSERT_FALSE(model.visible());
}

TEST(MonitorModelTest, Type)
{
    MonitorModel model;
    ASSERT_EQ(model.type(), MonitorModel::Type::Invalid);
}

TEST(MonitorModelTest, X)
{
    MonitorModel model;
    Monitor monitor("", "");
    monitor.setX(65);
    model.init(&monitor);
    ASSERT_EQ(model.x(), 65);

    monitor.setX(-2);
    ASSERT_EQ(model.x(), -2);
}

TEST(MonitorModelTest, Y)
{
    MonitorModel model;
    Monitor monitor("", "");
    monitor.setY(15);
    model.init(&monitor);
    ASSERT_EQ(model.y(), 15);

    monitor.setY(-8);
    ASSERT_EQ(model.y(), -8);
}

TEST(MonitorModelTest, Width)
{
    MonitorModel model;
    Monitor monitor("", "");
    QSignalSpy spy(&model, &MonitorModel::widthChanged);
    monitor.setWidth(20);
    model.init(&monitor);
    ASSERT_EQ(model.width(), 20);

    monitor.setWidth(150);
    ASSERT_EQ(model.width(), 150);

    model.setWidth(87);
    ASSERT_EQ(model.width(), 87);
    ASSERT_EQ(monitor.width(), 87);
    ASSERT_EQ(spy.count(), 1);
}

TEST(MonitorModelTest, Height)
{
    MonitorModel model;
    Monitor monitor("", "");
    QSignalSpy spy(&model, &MonitorModel::heightChanged);
    monitor.setHeight(20);
    model.init(&monitor);
    ASSERT_EQ(model.height(), 20);

    monitor.setHeight(150);
    ASSERT_EQ(model.height(), 150);

    model.setHeight(87);
    ASSERT_EQ(model.height(), 87);
    ASSERT_EQ(monitor.height(), 87);
    ASSERT_EQ(spy.count(), 1);
}
