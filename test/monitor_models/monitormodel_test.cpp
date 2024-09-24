#include <QtTest/QSignalSpy>
#include <scratchcpp/monitor.h>
#include <scratchcpp/sprite.h>
#include <monitormodel.h>
#include <extensionmock.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

using ::testing::Return;

TEST(MonitorModelTest, Constructors)
{
    {
        MonitorModel model1;
        MonitorModel model2(&model1);
        ASSERT_EQ(model2.parent(), &model1);
    }

    {
        MonitorModel model1;
        MonitorModel model2(nullptr, &model1);
        ASSERT_EQ(model2.parent(), &model1);
    }
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
    ASSERT_FALSE(model.visible());

    monitor.autoPosition({});
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

TEST(MonitorModelTest, Color)
{
    {
        MonitorModel model;
        ASSERT_EQ(model.color(), Qt::green);
    }

    {
        MonitorModel model(nullptr, nullptr);
        ASSERT_EQ(model.color(), Qt::green);
    }

    ExtensionMock extension;

    {
        // Invalid
        EXPECT_CALL(extension, name()).WillOnce(Return(""));
        MonitorModel model(&extension);
        ASSERT_EQ(model.color(), Qt::green);
    }

    {
        // Motion
        EXPECT_CALL(extension, name()).WillOnce(Return("Motion"));
        MonitorModel model(&extension);
        ASSERT_EQ(model.color(), QColor::fromString("#4C97FF"));
    }

    {
        // Looks
        EXPECT_CALL(extension, name()).WillOnce(Return("Looks"));
        MonitorModel model(&extension);
        ASSERT_EQ(model.color(), QColor::fromString("#9966FF"));
    }

    {
        // Sound
        EXPECT_CALL(extension, name()).WillOnce(Return("Sound"));
        MonitorModel model(&extension);
        ASSERT_EQ(model.color(), QColor::fromString("#CF63CF"));
    }

    {
        // Variables
        EXPECT_CALL(extension, name()).WillOnce(Return("Variables"));
        MonitorModel model(&extension);
        ASSERT_EQ(model.color(), QColor::fromString("#FF8C1A"));
    }

    {
        // Lists
        EXPECT_CALL(extension, name()).WillOnce(Return("Lists"));
        MonitorModel model(&extension);
        ASSERT_EQ(model.color(), QColor::fromString("#FF661A"));
    }
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

    QSignalSpy xSpy(&model, &MonitorModel::xChanged);
    QSignalSpy visibleSpy(&model, &MonitorModel::visibleChanged);

    model.onXChanged(-2);
    ASSERT_EQ(xSpy.count(), 1);
    ASSERT_EQ(visibleSpy.count(), 1);
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

    QSignalSpy ySpy(&model, &MonitorModel::yChanged);
    QSignalSpy visibleSpy(&model, &MonitorModel::visibleChanged);

    model.onYChanged(-8);
    ASSERT_EQ(model.y(), -8);
    ASSERT_EQ(ySpy.count(), 1);
    ASSERT_EQ(visibleSpy.count(), 1);
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
