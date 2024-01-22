#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <blocks/penblocks.h>
#include <enginemock.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

class PenBlocksTest : public testing::Test
{
    public:
        void SetUp() override { m_section = std::make_unique<PenBlocks>(); }

        void addValueInput(std::shared_ptr<Block> block, const std::string &name, PenBlocks::Inputs id, const Value &value) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setPrimaryValue(value);
            input->setInputId(id);
            block->addInput(input);
        }

        std::unique_ptr<IBlockSection> m_section;
        EngineMock m_engineMock;
};

TEST_F(PenBlocksTest, Name)
{
    ASSERT_EQ(m_section->name(), "Pen");
}

TEST_F(PenBlocksTest, CategoryVisible)
{
    ASSERT_TRUE(m_section->categoryVisible());
}

TEST_F(PenBlocksTest, RegisterBlocks)
{
    m_section->registerBlocks(&m_engineMock);
}
