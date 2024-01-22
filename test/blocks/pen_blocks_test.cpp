#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <penlayer.h>
#include <blocks/penblocks.h>
#include <enginemock.h>
#include <penlayermock.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

using ::testing::Return;

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
    // Blocks
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "pen_clear", &PenBlocks::compileClear));

    m_section->registerBlocks(&m_engineMock);
}

TEST_F(PenBlocksTest, Clear)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "pen_clear");

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::clear)).WillOnce(Return(2));
    compiler.init();
    compiler.setBlock(block);
    PenBlocks::compileClear(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 2, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(PenBlocksTest, ClearImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &PenBlocks::clear };

    PenLayerMock penLayer;
    PenLayer::addPenLayer(&m_engineMock, &penLayer);

    VirtualMachine vm(nullptr, &m_engineMock, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);

    EXPECT_CALL(penLayer, clear());
    EXPECT_CALL(m_engineMock, requestRedraw());
    vm.run();

    ASSERT_EQ(vm.registerCount(), 0);
}
