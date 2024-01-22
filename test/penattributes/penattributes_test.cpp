#include <penattributes.h>

#include "../common.h"

using namespace scratchcpprender;

TEST(PenAttributesTest, DefaultPenAttributes)
{
    PenAttributes attr;
    ASSERT_EQ(attr.color.redF(), 0);
    ASSERT_EQ(attr.color.greenF(), 0);
    ASSERT_EQ(attr.color.blueF(), 1);
    ASSERT_EQ(attr.color.alphaF(), 1);
    ASSERT_EQ(attr.diameter, 1);
}
