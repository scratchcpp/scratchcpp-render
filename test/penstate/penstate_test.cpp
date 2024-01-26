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

TEST(PenStateTest, UpdateColor)
{
    PenState state;
    state.color = 78.64;
    state.saturation = 45.07;
    state.brightness = 12.5;
    state.transparency = 36.09;
    state.shade = 85;
    state.updateColor();
    ASSERT_EQ(state.penAttributes.color, QColor::fromHsv(283, 114, 31, 162));
}
