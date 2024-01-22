#include <blocks/penextension.h>
#include <blocks/penblocks.h>
#include <enginemock.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

using ::testing::WithArgs;
using ::testing::Invoke;
using ::testing::_;

TEST(PenExtensionTest, Name)
{
    PenExtension ext;
    ASSERT_EQ(ext.name(), "pen");
}

TEST(PenExtensionTest, Description)
{
    PenExtension ext;
    ASSERT_EQ(ext.description(), "Pen extension");
}

TEST(PenExtensionTest, IncludeByDefault)
{
    PenExtension ext;
    ASSERT_FALSE(ext.includeByDefault());
}

TEST(PenExtensionTest, RegisterSections)
{
    PenExtension ext;
    EngineMock engine;

    EXPECT_CALL(engine, registerSection(_)).WillOnce(WithArgs<0>(Invoke([](std::shared_ptr<IBlockSection> section) { ASSERT_TRUE(dynamic_cast<PenBlocks *>(section.get())); })));
    ext.registerSections(&engine);
}
