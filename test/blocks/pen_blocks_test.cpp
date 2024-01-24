#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <penlayer.h>
#include <spritemodel.h>
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

        void addObscuredInput(std::shared_ptr<Block> block, const std::string &name, PenBlocks::Inputs id, std::shared_ptr<Block> valueBlock) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::ObscuredShadow);
            input->setValueBlock(valueBlock);
            input->setInputId(id);
            block->addInput(input);
        }

        std::shared_ptr<Block> createNullBlock(const std::string &id)
        {
            std::shared_ptr<Block> block = std::make_shared<Block>(id, "");
            BlockComp func = [](Compiler *compiler) { compiler->addInstruction(vm::OP_NULL); };
            block->setCompileFunction(func);

            return block;
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
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "pen_penDown", &PenBlocks::compilePenDown));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "pen_penUp", &PenBlocks::compilePenUp));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "pen_setPenColorToColor", &PenBlocks::compileSetPenColorToColor));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "pen_changePenSizeBy", &PenBlocks::compileChangePenSizeBy));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "pen_setPenSizeTo", &PenBlocks::compileSetPenSizeTo));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_section.get(), "pen_changePenHueBy", &PenBlocks::compileChangePenHueBy));

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "COLOR", PenBlocks::COLOR));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "SIZE", PenBlocks::SIZE));
    EXPECT_CALL(m_engineMock, addInput(m_section.get(), "HUE", PenBlocks::HUE));

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

TEST_F(PenBlocksTest, PenDown)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "pen_penDown");

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::penDown)).WillOnce(Return(2));
    compiler.init();
    compiler.setBlock(block);
    PenBlocks::compilePenDown(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 2, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(PenBlocksTest, PenDownImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &PenBlocks::penDown };

    SpriteModel model;
    Sprite sprite;
    sprite.setInterface(&model);

    VirtualMachine vm(&sprite, &m_engineMock, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);

    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(model.penDown());

    vm.reset();
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_TRUE(model.penDown());
}

TEST_F(PenBlocksTest, PenUp)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "pen_penUp");

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::penUp)).WillOnce(Return(2));
    compiler.init();
    compiler.setBlock(block);
    PenBlocks::compilePenUp(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 2, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(PenBlocksTest, PenUpImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &PenBlocks::penUp };

    SpriteModel model;
    model.setPenDown(true);
    Sprite sprite;
    sprite.setInterface(&model);

    VirtualMachine vm(&sprite, &m_engineMock, nullptr);
    vm.setBytecode(bytecode);
    vm.setFunctions(functions);

    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_FALSE(model.penDown());

    vm.reset();
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_FALSE(model.penDown());
}

TEST_F(PenBlocksTest, SetPenColorToColor)
{
    Compiler compiler(&m_engineMock);

    // set pen color to ("#AABBCC")
    auto block1 = std::make_shared<Block>("a", "pen_setPenColorToColor");
    addValueInput(block1, "COLOR", PenBlocks::COLOR, "#AABBCC");

    // set pen color to (null block)
    auto block2 = std::make_shared<Block>("b", "pen_setPenColorToColor");
    addObscuredInput(block2, "COLOR", PenBlocks::COLOR, createNullBlock("c"));

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::setPenColorToColor)).WillOnce(Return(2));
    compiler.setBlock(block1);
    PenBlocks::compileSetPenColorToColor(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::setPenColorToColor)).WillOnce(Return(2));
    compiler.setBlock(block2);
    PenBlocks::compileSetPenColorToColor(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 2, vm::OP_NULL, vm::OP_EXEC, 2, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toString(), "#AABBCC");
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(PenBlocksTest, SetPenColorToColorImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_CONST, 4, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode6[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode7[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &PenBlocks::setPenColorToColor };
    static Value constValues[] = { "#AABbCC", "#03F", "#FFGFFF", "#AABBCCDD", "FFFFFF", 1228097602, 255 };

    SpriteModel model;
    Sprite sprite;
    sprite.setInterface(&model);

    VirtualMachine vm(&sprite, &m_engineMock, nullptr);
    vm.setBytecode(bytecode1);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().color, QColor(170, 187, 204));

    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().color, QColor(0, 51, 255));

    vm.reset();
    vm.setBytecode(bytecode3);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().color, QColor(0, 0, 0));

    vm.reset();
    vm.setBytecode(bytecode4);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().color, QColor(0, 0, 0));

    vm.reset();
    vm.setBytecode(bytecode5);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().color, QColor(0, 0, 0));

    vm.reset();
    vm.setBytecode(bytecode6);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().color, QColor::fromRgba(1228097602));

    vm.reset();
    vm.setBytecode(bytecode7);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().color, QColor::fromRgb(255));
}

TEST_F(PenBlocksTest, ChangePenSizeBy)
{
    Compiler compiler(&m_engineMock);

    // change pen size by (4.5)
    auto block1 = std::make_shared<Block>("a", "pen_changePenSizeBy");
    addValueInput(block1, "SIZE", PenBlocks::SIZE, 4.5);

    // change pen size by (null block)
    auto block2 = std::make_shared<Block>("b", "pen_changePenSizeBy");
    addObscuredInput(block2, "SIZE", PenBlocks::SIZE, createNullBlock("c"));

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::changePenSizeBy)).WillOnce(Return(2));
    compiler.setBlock(block1);
    PenBlocks::compileChangePenSizeBy(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::changePenSizeBy)).WillOnce(Return(2));
    compiler.setBlock(block2);
    PenBlocks::compileChangePenSizeBy(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 2, vm::OP_NULL, vm::OP_EXEC, 2, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 4.5);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(PenBlocksTest, ChangePenSizeByImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &PenBlocks::changePenSizeBy };
    static Value constValues[] = { 511.5, -650.08 };

    SpriteModel model;
    Sprite sprite;
    sprite.setInterface(&model);

    VirtualMachine vm(&sprite, &m_engineMock, nullptr);
    vm.setBytecode(bytecode1);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().diameter, 512.5);

    vm.reset();
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().diameter, 1024);

    vm.reset();
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().diameter, 1200);

    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().diameter, 549.92);

    vm.reset();
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().diameter, 1);
}

TEST_F(PenBlocksTest, SetPenSizeTo)
{
    Compiler compiler(&m_engineMock);

    // set pen size to (51.46)
    auto block1 = std::make_shared<Block>("a", "pen_setPenSizeTo");
    addValueInput(block1, "SIZE", PenBlocks::SIZE, 51.46);

    // set pen size to (null block)
    auto block2 = std::make_shared<Block>("b", "pen_setPenSizeTo");
    addObscuredInput(block2, "SIZE", PenBlocks::SIZE, createNullBlock("c"));

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::setPenSizeTo)).WillOnce(Return(2));
    compiler.setBlock(block1);
    PenBlocks::compileSetPenSizeTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::setPenSizeTo)).WillOnce(Return(2));
    compiler.setBlock(block2);
    PenBlocks::compileSetPenSizeTo(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 2, vm::OP_NULL, vm::OP_EXEC, 2, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 51.46);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(PenBlocksTest, SetPenSizeToImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &PenBlocks::setPenSizeTo };
    static Value constValues[] = { 511.5, -650.08, 1500 };

    SpriteModel model;
    Sprite sprite;
    sprite.setInterface(&model);

    VirtualMachine vm(&sprite, &m_engineMock, nullptr);
    vm.setBytecode(bytecode1);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().diameter, 511.5);

    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().diameter, 1);

    vm.reset();
    vm.setBytecode(bytecode3);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().diameter, 1200);
}

TEST_F(PenBlocksTest, ChangePenHueBy)
{
    Compiler compiler(&m_engineMock);

    // change pen hue by (4.5)
    auto block1 = std::make_shared<Block>("a", "pen_changePenHueBy");
    addValueInput(block1, "HUE", PenBlocks::HUE, 4.5);

    // change pen hue by (null block)
    auto block2 = std::make_shared<Block>("b", "pen_changePenHueBy");
    addObscuredInput(block2, "HUE", PenBlocks::HUE, createNullBlock("c"));

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::changePenHueBy)).WillOnce(Return(2));
    compiler.setBlock(block1);
    PenBlocks::compileChangePenHueBy(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::changePenHueBy)).WillOnce(Return(2));
    compiler.setBlock(block2);
    PenBlocks::compileChangePenHueBy(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 2, vm::OP_NULL, vm::OP_EXEC, 2, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 4.5);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(PenBlocksTest, ChangePenHueByImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &PenBlocks::changePenHueBy };
    static Value constValues[] = { 125.7, -114.09 };

    SpriteModel model;
    QColor color = model.penAttributes().color;
    color.setAlpha(150);
    model.penState().setColor(color);
    Sprite sprite;
    sprite.setInterface(&model);

    VirtualMachine vm(&sprite, &m_engineMock, nullptr);
    vm.setBytecode(bytecode1);
    vm.setFunctions(functions);
    vm.setConstValues(constValues);

    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().color, QColor::fromHsv(103, 255, 255, 150));

    vm.reset();
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().color, QColor::fromHsv(329, 255, 255, 150));

    vm.reset();
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().color, QColor::fromHsv(192, 255, 255, 150));

    vm.reset();
    vm.setBytecode(bytecode2);
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().color, QColor::fromHsv(350, 255, 255, 150));

    vm.reset();
    vm.run();
    ASSERT_EQ(vm.registerCount(), 0);
    ASSERT_EQ(model.penAttributes().color, QColor::fromHsv(145, 255, 255, 150));
}
