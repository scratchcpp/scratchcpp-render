#include <penstate.h>

#include "../common.h"

using namespace scratchcpprender;

TEST(PenStateTest, DefaultPenState)
{
    PenState state;
    ASSERT_FALSE(state.penDown);
    ASSERT_EQ(state.color, 66.66);
    ASSERT_EQ(state.saturation, 100);
    ASSERT_EQ(state.brightness, 100);
    ASSERT_EQ(state.transparency, 0);
    ASSERT_EQ(state.shade, 50);

    PenAttributes defaultAttributes;
    ASSERT_EQ(state.penAttributes.color, defaultAttributes.color);
    ASSERT_EQ(state.penAttributes.diameter, defaultAttributes.diameter);
}

TEST(PenStateTest, SetColor)
{
    PenState state;
    QColor color(64, 20, 189, 167);
    state.setColor(color);
    ASSERT_FALSE(state.penDown);
    ASSERT_EQ(std::round(state.color * 100) / 100, 70.83);
    ASSERT_EQ(std::round(state.saturation * 100) / 100, 89.42);
    ASSERT_EQ(std::round(state.brightness * 100) / 100, 74.12);
    ASSERT_EQ(std::round(state.transparency * 100) / 100, 34.51);
    ASSERT_EQ(state.shade, 50);
}
