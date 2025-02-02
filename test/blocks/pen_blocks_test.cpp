#include <scratchcpp/compiler.h>
#include <scratchcpp/block.h>
#include <scratchcpp/input.h>
#include <scratchcpp/field.h>
#include <penlayer.h>
#include <spritemodel.h>
#include <stagemodel.h>
#include <blocks/penblocks.h>
#include <enginemock.h>
#include <penlayermock.h>
#include <renderedtargetmock.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

using ::testing::Return;

class PenBlocksTest : public testing::Test
{
    public:
        void SetUp() override { m_extension = std::make_unique<PenBlocks>(); }

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

        std::shared_ptr<Input> addNullInput(std::shared_ptr<Block> block, const std::string &name, PenBlocks::Inputs id) const
        {
            auto input = std::make_shared<Input>(name, Input::Type::Shadow);
            input->setInputId(id);
            block->addInput(input);

            return input;
        }

        void addDropdownInput(std::shared_ptr<Block> block, const std::string &name, PenBlocks::Inputs id, const std::string &selectedValue, std::shared_ptr<Block> valueBlock = nullptr) const
        {
            if (valueBlock)
                addObscuredInput(block, name, id, valueBlock);
            else {
                auto input = addNullInput(block, name, id);
                auto menu = std::make_shared<Block>(block->id() + "_menu", block->opcode() + "_menu");
                menu->setShadow(true);
                input->setValueBlock(menu);
                addDropdownField(menu, name, -1, selectedValue, -1);
            }
        }

        void addDropdownField(std::shared_ptr<Block> block, const std::string &name, int id, const std::string &value, int valueId) const
        {
            auto field = std::make_shared<Field>(name, value);
            field->setFieldId(id);
            field->setSpecialValueId(valueId);
            block->addField(field);
        }

        std::shared_ptr<Block> createNullBlock(const std::string &id)
        {
            std::shared_ptr<Block> block = std::make_shared<Block>(id, "");
            BlockComp func = [](Compiler *compiler) { compiler->addInstruction(vm::OP_NULL); };
            block->setCompileFunction(func);

            return block;
        }

        std::unique_ptr<IExtension> m_extension;
        EngineMock m_engineMock;
};

TEST_F(PenBlocksTest, Name)
{
    ASSERT_EQ(m_extension->name(), "pen");
}

TEST_F(PenBlocksTest, RegisterBlocks)
{
    // Blocks
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "pen_clear", &PenBlocks::compileClear));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "pen_stamp", &PenBlocks::compileStamp));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "pen_penDown", &PenBlocks::compilePenDown));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "pen_penUp", &PenBlocks::compilePenUp));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "pen_setPenColorToColor", &PenBlocks::compileSetPenColorToColor));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "pen_changePenColorParamBy", &PenBlocks::compileChangePenColorParamBy));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "pen_setPenColorParamTo", &PenBlocks::compileSetPenColorParamTo));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "pen_changePenSizeBy", &PenBlocks::compileChangePenSizeBy));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "pen_setPenSizeTo", &PenBlocks::compileSetPenSizeTo));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "pen_changePenShadeBy", &PenBlocks::compileChangePenShadeBy));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "pen_setPenShadeToNumber", &PenBlocks::compileSetPenShadeToNumber));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "pen_changePenHueBy", &PenBlocks::compileChangePenHueBy));
    EXPECT_CALL(m_engineMock, addCompileFunction(m_extension.get(), "pen_setPenHueToNumber", &PenBlocks::compileSetPenHueToNumber));

    // Inputs
    EXPECT_CALL(m_engineMock, addInput(m_extension.get(), "COLOR", PenBlocks::COLOR));
    EXPECT_CALL(m_engineMock, addInput(m_extension.get(), "COLOR_PARAM", PenBlocks::COLOR_PARAM));
    EXPECT_CALL(m_engineMock, addInput(m_extension.get(), "VALUE", PenBlocks::VALUE));
    EXPECT_CALL(m_engineMock, addInput(m_extension.get(), "SIZE", PenBlocks::SIZE));
    EXPECT_CALL(m_engineMock, addInput(m_extension.get(), "SHADE", PenBlocks::SHADE));
    EXPECT_CALL(m_engineMock, addInput(m_extension.get(), "HUE", PenBlocks::HUE));

    m_extension->registerBlocks(&m_engineMock);
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

TEST_F(PenBlocksTest, Stamp)
{
    Compiler compiler(&m_engineMock);

    auto block = std::make_shared<Block>("a", "pen_stamp");

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::stamp)).WillOnce(Return(2));
    compiler.init();
    compiler.setBlock(block);
    PenBlocks::compileStamp(&compiler);
    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_EXEC, 2, vm::OP_HALT }));
    ASSERT_TRUE(compiler.constValues().empty());
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(PenBlocksTest, StampImpl)
{
    static unsigned int bytecode[] = { vm::OP_START, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &PenBlocks::stamp };

    PenLayerMock penLayer;
    PenLayer::addPenLayer(&m_engineMock, &penLayer);
    RenderedTargetMock renderedTarget;

    // Test sprite
    libscratchcpp::Sprite sprite;
    SpriteModel spriteModel;
    sprite.setInterface(&spriteModel);
    spriteModel.setRenderedTarget(&renderedTarget);

    VirtualMachine vm1(&sprite, &m_engineMock, nullptr);
    vm1.setBytecode(bytecode);
    vm1.setFunctions(functions);

    EXPECT_CALL(penLayer, stamp(&renderedTarget));
    EXPECT_CALL(m_engineMock, requestRedraw());
    vm1.run();

    ASSERT_EQ(vm1.registerCount(), 0);

    // Test stage
    libscratchcpp::Stage stage;
    StageModel stageModel;
    stage.setInterface(&stageModel);
    stageModel.setRenderedTarget(&renderedTarget);

    VirtualMachine vm2(&stage, &m_engineMock, nullptr);
    vm2.setBytecode(bytecode);
    vm2.setFunctions(functions);

    EXPECT_CALL(penLayer, stamp(&renderedTarget));
    EXPECT_CALL(m_engineMock, requestRedraw());
    vm2.run();

    ASSERT_EQ(vm2.registerCount(), 0);
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

    std::vector<std::shared_ptr<TargetModel>> models;
    std::vector<std::shared_ptr<Target>> targets;

    models.push_back(std::make_shared<SpriteModel>());
    targets.push_back(std::make_shared<Sprite>());
    static_cast<Sprite *>(targets.back().get())->setInterface(static_cast<SpriteModel *>(models.back().get()));

    models.push_back(std::make_shared<StageModel>());
    targets.push_back(std::make_shared<Stage>());
    static_cast<Stage *>(targets.back().get())->setInterface(static_cast<StageModel *>(models.back().get()));

    for (int i = 0; i < targets.size(); i++) {
        auto target = targets[i];
        auto model = models[i];

        VirtualMachine vm(target.get(), &m_engineMock, nullptr);
        vm.setBytecode(bytecode);
        vm.setFunctions(functions);

        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_TRUE(model->penDown());

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_TRUE(model->penDown());
    }
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

    std::vector<std::shared_ptr<TargetModel>> models;
    std::vector<std::shared_ptr<Target>> targets;

    models.push_back(std::make_shared<SpriteModel>());
    targets.push_back(std::make_shared<Sprite>());
    static_cast<Sprite *>(targets.back().get())->setInterface(static_cast<SpriteModel *>(models.back().get()));

    models.push_back(std::make_shared<StageModel>());
    targets.push_back(std::make_shared<Stage>());
    static_cast<Stage *>(targets.back().get())->setInterface(static_cast<StageModel *>(models.back().get()));

    for (int i = 0; i < targets.size(); i++) {
        auto target = targets[i];
        auto model = models[i];
        model->setPenDown(true);

        VirtualMachine vm(target.get(), &m_engineMock, nullptr);
        vm.setBytecode(bytecode);
        vm.setFunctions(functions);

        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_FALSE(model->penDown());

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_FALSE(model->penDown());
    }
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

    std::vector<std::shared_ptr<TargetModel>> models;
    std::vector<std::shared_ptr<Target>> targets;

    models.push_back(std::make_shared<SpriteModel>());
    targets.push_back(std::make_shared<Sprite>());
    static_cast<Sprite *>(targets.back().get())->setInterface(static_cast<SpriteModel *>(models.back().get()));

    models.push_back(std::make_shared<StageModel>());
    targets.push_back(std::make_shared<Stage>());
    static_cast<Stage *>(targets.back().get())->setInterface(static_cast<StageModel *>(models.back().get()));

    for (int i = 0; i < targets.size(); i++) {
        auto target = targets[i];
        auto model = models[i];

        VirtualMachine vm(target.get(), &m_engineMock, nullptr);
        vm.setBytecode(bytecode1);
        vm.setFunctions(functions);
        vm.setConstValues(constValues);

        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(210, 42, 204)));

        vm.reset();
        vm.setBytecode(bytecode2);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(228, 255, 255)));

        vm.reset();
        vm.setBytecode(bytecode3);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(359, 0, 0)));

        vm.reset();
        vm.setBytecode(bytecode4);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(359, 0, 0)));

        vm.reset();
        vm.setBytecode(bytecode5);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(359, 0, 0)));

        vm.reset();
        vm.setBytecode(bytecode6);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(162, 74, 72, 73)));

        vm.reset();
        vm.setBytecode(bytecode7);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(239, 255, 255)));
    }
}

TEST_F(PenBlocksTest, ChangePenColorParamBy)
{
    Compiler compiler(&m_engineMock);

    // change pen (color) by (34.6)
    auto block1 = std::make_shared<Block>("a", "pen_changePenColorParamBy");
    addDropdownInput(block1, "COLOR_PARAM", PenBlocks::COLOR_PARAM, "color");
    addValueInput(block1, "VALUE", PenBlocks::VALUE, 34.6);

    // change pen (saturation) by (46.8)
    auto block2 = std::make_shared<Block>("b", "pen_changePenColorParamBy");
    addDropdownInput(block2, "COLOR_PARAM", PenBlocks::COLOR_PARAM, "saturation");
    addValueInput(block2, "VALUE", PenBlocks::VALUE, 46.8);

    // change pen (brightness) by (0.45)
    auto block3 = std::make_shared<Block>("c", "pen_changePenColorParamBy");
    addDropdownInput(block3, "COLOR_PARAM", PenBlocks::COLOR_PARAM, "brightness");
    addValueInput(block3, "VALUE", PenBlocks::VALUE, 0.45);

    // change pen (transparency) by (89.06)
    auto block4 = std::make_shared<Block>("d", "pen_changePenColorParamBy");
    addDropdownInput(block4, "COLOR_PARAM", PenBlocks::COLOR_PARAM, "transparency");
    addValueInput(block4, "VALUE", PenBlocks::VALUE, 89.06);

    // change pen (invalid param) by (52.7)
    auto block5 = std::make_shared<Block>("e", "pen_changePenColorParamBy");
    addDropdownInput(block5, "COLOR_PARAM", PenBlocks::COLOR_PARAM, "invalid param");
    addValueInput(block5, "VALUE", PenBlocks::VALUE, 52.7);

    // change pen (null block) by (35.2)
    auto block6 = std::make_shared<Block>("f", "pen_changePenColorParamBy");
    addDropdownInput(block6, "COLOR_PARAM", PenBlocks::COLOR_PARAM, "", createNullBlock("g"));
    addValueInput(block6, "VALUE", PenBlocks::VALUE, 35.2);

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::changePenColorBy)).WillOnce(Return(0));
    compiler.setBlock(block1);
    PenBlocks::compileChangePenColorParamBy(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::changePenSaturationBy)).WillOnce(Return(1));
    compiler.setBlock(block2);
    PenBlocks::compileChangePenColorParamBy(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::changePenBrightnessBy)).WillOnce(Return(2));
    compiler.setBlock(block3);
    PenBlocks::compileChangePenColorParamBy(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::changePenTransparencyBy)).WillOnce(Return(3));
    compiler.setBlock(block4);
    PenBlocks::compileChangePenColorParamBy(&compiler);

    compiler.setBlock(block5);
    PenBlocks::compileChangePenColorParamBy(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::changePenColorParamBy)).WillOnce(Return(4));
    compiler.setBlock(block6);
    PenBlocks::compileChangePenColorParamBy(&compiler);

    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_CONST, 2, vm::OP_EXEC, 2, vm::OP_CONST, 3, vm::OP_EXEC, 3,
              vm::OP_NULL,  vm::OP_CONST, 4, vm::OP_EXEC, 4, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 34.6, 46.8, 0.45, 89.06, 35.2 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(PenBlocksTest, ChangePenColorParamByImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 5, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 6, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_CONST, 5, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_CONST, 6, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_CONST, 5, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode6[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_CONST, 6, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode7[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_CONST, 5, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode8[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_CONST, 6, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode9[] = { vm::OP_START, vm::OP_CONST, 4, vm::OP_CONST, 5, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode10[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode11[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode12[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 2, vm::OP_HALT };
    static unsigned int bytecode13[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 2, vm::OP_HALT };
    static unsigned int bytecode14[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 3, vm::OP_HALT };
    static unsigned int bytecode15[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 3, vm::OP_HALT };
    static unsigned int bytecode16[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 4, vm::OP_HALT };
    static unsigned int bytecode17[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 4, vm::OP_HALT };
    static BlockFunc
        functions[] = { &PenBlocks::changePenColorParamBy, &PenBlocks::changePenColorBy, &PenBlocks::changePenSaturationBy, &PenBlocks::changePenBrightnessBy, &PenBlocks::changePenTransparencyBy };
    static Value constValues[] = { "color", "saturation", "brightness", "transparency", "invalid", 53.2, -120.8 };

    std::vector<std::shared_ptr<TargetModel>> models;
    std::vector<std::shared_ptr<Target>> targets;

    models.push_back(std::make_shared<SpriteModel>());
    targets.push_back(std::make_shared<Sprite>());
    static_cast<Sprite *>(targets.back().get())->setInterface(static_cast<SpriteModel *>(models.back().get()));

    models.push_back(std::make_shared<StageModel>());
    targets.push_back(std::make_shared<Stage>());
    static_cast<Stage *>(targets.back().get())->setInterface(static_cast<StageModel *>(models.back().get()));

    for (int i = 0; i < targets.size(); i++) {
        auto target = targets[i];
        auto model = models[i];
        model->penState().transparency = 100 * (1 - 150 / 255.0);

        VirtualMachine vm(target.get(), &m_engineMock, nullptr);
        vm.setBytecode(bytecode1);
        vm.setFunctions(functions);
        vm.setConstValues(constValues);

        // color
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(71, 255, 255, 150)));

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(263, 255, 255, 150)));

        vm.reset();
        vm.setBytecode(bytecode2);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 255, 255, 150)));

        // saturation
        model->penState().saturation = 32.4;
        vm.reset();
        vm.setBytecode(bytecode3);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 218, 255, 150)));

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 255, 255, 150)));

        vm.reset();
        vm.setBytecode(bytecode4);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 0, 255, 150)));

        // brightness
        model->penState().brightness = 12.5;
        vm.reset();
        vm.setBytecode(bytecode5);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 0, 167, 150)));

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 0, 255, 150)));

        vm.reset();
        vm.setBytecode(bytecode6);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 0, 0, 150)));

        // transparency
        model->penState().transparency = 6.28;
        vm.reset();
        vm.setBytecode(bytecode7);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 0, 0, 103)));

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 0, 0, 0)));

        vm.reset();
        vm.setBytecode(bytecode8);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 0, 0, 255)));

        // invalid parameter
        vm.reset();
        vm.setBytecode(bytecode9);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 0, 0, 255)));

        // color (optimized)
        model->penState() = PenState();
        model->penState().transparency = 100 * (1 - 150 / 255.0);
        vm.reset();
        vm.setBytecode(bytecode10);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(71, 255, 255, 150)));

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(263, 255, 255, 150)));

        vm.reset();
        vm.setBytecode(bytecode11);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 255, 255, 150)));

        // saturation (optimized)
        model->penState().saturation = 32.4;
        vm.reset();
        vm.setBytecode(bytecode12);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 218, 255, 150)));

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 255, 255, 150)));

        vm.reset();
        vm.setBytecode(bytecode13);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 0, 255, 150)));

        // brightness (optimized)
        model->penState().brightness = 12.5;
        vm.reset();
        vm.setBytecode(bytecode14);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 0, 167, 150)));

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 0, 255, 150)));

        vm.reset();
        vm.setBytecode(bytecode15);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 0, 0, 150)));

        // transparency (optimized)
        model->penState().transparency = 6.28;
        vm.reset();
        vm.setBytecode(bytecode16);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 0, 0, 103)));

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 0, 0, 0)));

        vm.reset();
        vm.setBytecode(bytecode17);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(188, 0, 0, 255)));
    }
}

TEST_F(PenBlocksTest, SetPenColorParamTo)
{
    Compiler compiler(&m_engineMock);

    // set pen (color) to (34.6)
    auto block1 = std::make_shared<Block>("a", "pen_setPenColorParamTo");
    addDropdownInput(block1, "COLOR_PARAM", PenBlocks::COLOR_PARAM, "color");
    addValueInput(block1, "VALUE", PenBlocks::VALUE, 34.6);

    // set pen (saturation) to (46.8)
    auto block2 = std::make_shared<Block>("b", "pen_setPenColorParamTo");
    addDropdownInput(block2, "COLOR_PARAM", PenBlocks::COLOR_PARAM, "saturation");
    addValueInput(block2, "VALUE", PenBlocks::VALUE, 46.8);

    // set pen (brightness) to (0.45)
    auto block3 = std::make_shared<Block>("c", "pen_setPenColorParamTo");
    addDropdownInput(block3, "COLOR_PARAM", PenBlocks::COLOR_PARAM, "brightness");
    addValueInput(block3, "VALUE", PenBlocks::VALUE, 0.45);

    // set pen (transparency) to (89.06)
    auto block4 = std::make_shared<Block>("d", "pen_setPenColorParamTo");
    addDropdownInput(block4, "COLOR_PARAM", PenBlocks::COLOR_PARAM, "transparency");
    addValueInput(block4, "VALUE", PenBlocks::VALUE, 89.06);

    // set pen (invalid param) to (52.7)
    auto block5 = std::make_shared<Block>("e", "pen_setPenColorParamTo");
    addDropdownInput(block5, "COLOR_PARAM", PenBlocks::COLOR_PARAM, "invalid param");
    addValueInput(block5, "VALUE", PenBlocks::VALUE, 52.7);

    // set pen (null block) to (35.2)
    auto block6 = std::make_shared<Block>("f", "pen_setPenColorParamTo");
    addDropdownInput(block6, "COLOR_PARAM", PenBlocks::COLOR_PARAM, "", createNullBlock("g"));
    addValueInput(block6, "VALUE", PenBlocks::VALUE, 35.2);

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::setPenColorTo)).WillOnce(Return(0));
    compiler.setBlock(block1);
    PenBlocks::compileSetPenColorParamTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::setPenSaturationTo)).WillOnce(Return(1));
    compiler.setBlock(block2);
    PenBlocks::compileSetPenColorParamTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::setPenBrightnessTo)).WillOnce(Return(2));
    compiler.setBlock(block3);
    PenBlocks::compileSetPenColorParamTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::setPenTransparencyTo)).WillOnce(Return(3));
    compiler.setBlock(block4);
    PenBlocks::compileSetPenColorParamTo(&compiler);

    compiler.setBlock(block5);
    PenBlocks::compileSetPenColorParamTo(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::setPenColorParamTo)).WillOnce(Return(4));
    compiler.setBlock(block6);
    PenBlocks::compileSetPenColorParamTo(&compiler);

    compiler.end();

    ASSERT_EQ(
        compiler.bytecode(),
        std::vector<unsigned int>(
            { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_CONST, 1, vm::OP_EXEC, 1, vm::OP_CONST, 2, vm::OP_EXEC, 2, vm::OP_CONST, 3, vm::OP_EXEC, 3,
              vm::OP_NULL,  vm::OP_CONST, 4, vm::OP_EXEC, 4, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues(), std::vector<Value>({ 34.6, 46.8, 0.45, 89.06, 35.2 }));
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(PenBlocksTest, SetPenColorParamToImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 5, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 6, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_CONST, 7, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode4[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_CONST, 5, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode5[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_CONST, 6, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode6[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_CONST, 7, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode7[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_CONST, 5, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode8[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_CONST, 6, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode9[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_CONST, 7, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode10[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_CONST, 5, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode11[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_CONST, 6, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode12[] = { vm::OP_START, vm::OP_CONST, 3, vm::OP_CONST, 7, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode13[] = { vm::OP_START, vm::OP_CONST, 4, vm::OP_CONST, 5, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode14[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode15[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode16[] = { vm::OP_START, vm::OP_CONST, 7, vm::OP_EXEC, 1, vm::OP_HALT };
    static unsigned int bytecode17[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 2, vm::OP_HALT };
    static unsigned int bytecode18[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 2, vm::OP_HALT };
    static unsigned int bytecode19[] = { vm::OP_START, vm::OP_CONST, 7, vm::OP_EXEC, 2, vm::OP_HALT };
    static unsigned int bytecode20[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 3, vm::OP_HALT };
    static unsigned int bytecode21[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 3, vm::OP_HALT };
    static unsigned int bytecode22[] = { vm::OP_START, vm::OP_CONST, 7, vm::OP_EXEC, 3, vm::OP_HALT };
    static unsigned int bytecode23[] = { vm::OP_START, vm::OP_CONST, 5, vm::OP_EXEC, 4, vm::OP_HALT };
    static unsigned int bytecode24[] = { vm::OP_START, vm::OP_CONST, 6, vm::OP_EXEC, 4, vm::OP_HALT };
    static unsigned int bytecode25[] = { vm::OP_START, vm::OP_CONST, 7, vm::OP_EXEC, 4, vm::OP_HALT };
    static BlockFunc functions[] = { &PenBlocks::setPenColorParamTo, &PenBlocks::setPenColorTo, &PenBlocks::setPenSaturationTo, &PenBlocks::setPenBrightnessTo, &PenBlocks::setPenTransparencyTo };
    static Value constValues[] = { "color", "saturation", "brightness", "transparency", "invalid", 53.2, -234.9, 287.1 };

    std::vector<std::shared_ptr<TargetModel>> models;
    std::vector<std::shared_ptr<Target>> targets;

    models.push_back(std::make_shared<SpriteModel>());
    targets.push_back(std::make_shared<Sprite>());
    static_cast<Sprite *>(targets.back().get())->setInterface(static_cast<SpriteModel *>(models.back().get()));

    models.push_back(std::make_shared<StageModel>());
    targets.push_back(std::make_shared<Stage>());
    static_cast<Stage *>(targets.back().get())->setInterface(static_cast<StageModel *>(models.back().get()));

    for (int i = 0; i < targets.size(); i++) {
        auto target = targets[i];
        auto model = models[i];
        model->penState().color = 78.6;
        model->penState().transparency = 100 * (1 - 150 / 255.0);

        VirtualMachine vm(target.get(), &m_engineMock, nullptr);
        vm.setBytecode(bytecode1);
        vm.setFunctions(functions);
        vm.setConstValues(constValues);

        // color
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(191, 255, 255, 150)));

        vm.reset();
        vm.setBytecode(bytecode2);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(234, 255, 255, 150)));

        vm.reset();
        vm.setBytecode(bytecode3);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 255, 255, 150)));

        // saturation
        model->penState().saturation = 32.4;
        vm.reset();
        vm.setBytecode(bytecode4);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 135, 255, 150)));

        vm.reset();
        vm.setBytecode(bytecode5);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 0, 255, 150)));

        vm.reset();
        vm.setBytecode(bytecode6);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 255, 255, 150)));

        // brightness
        model->penState().brightness = 12.5;
        vm.reset();
        vm.setBytecode(bytecode7);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 255, 135, 150)));

        vm.reset();
        vm.setBytecode(bytecode8);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 255, 0, 150)));

        vm.reset();
        vm.setBytecode(bytecode9);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 255, 255, 150)));

        // transparency
        model->penState().transparency = 12.5;
        vm.reset();
        vm.setBytecode(bytecode10);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 255, 255, 119)));

        vm.reset();
        vm.setBytecode(bytecode11);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 255, 255, 255)));

        vm.reset();
        vm.setBytecode(bytecode12);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 255, 255, 0)));

        // invalid parameter
        vm.reset();
        vm.setBytecode(bytecode13);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 255, 255, 0)));

        // color (optimized)
        model->penState() = PenState();
        model->penState().color = 78.6;
        model->penState().transparency = 100 * (1 - 150 / 255.0);
        vm.reset();
        vm.setBytecode(bytecode14);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(191, 255, 255, 150)));

        vm.reset();
        vm.setBytecode(bytecode15);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(234, 255, 255, 150)));

        vm.reset();
        vm.setBytecode(bytecode16);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 255, 255, 150)));

        // saturation (optimized)
        model->penState().saturation = 32.4;
        vm.reset();
        vm.setBytecode(bytecode17);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 135, 255, 150)));

        vm.reset();
        vm.setBytecode(bytecode18);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 0, 255, 150)));

        vm.reset();
        vm.setBytecode(bytecode19);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 255, 255, 150)));

        // brightness (optimized)
        model->penState().brightness = 12.5;
        vm.reset();
        vm.setBytecode(bytecode20);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 255, 135, 150)));

        vm.reset();
        vm.setBytecode(bytecode21);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 255, 0, 150)));

        vm.reset();
        vm.setBytecode(bytecode22);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 255, 255, 150)));

        // transparency (optimized)
        model->penState().transparency = 12.5;
        vm.reset();
        vm.setBytecode(bytecode23);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 255, 255, 119)));

        vm.reset();
        vm.setBytecode(bytecode24);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 255, 255, 255)));

        vm.reset();
        vm.setBytecode(bytecode25);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(313, 255, 255, 0)));
    }
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

    std::vector<std::shared_ptr<TargetModel>> models;
    std::vector<std::shared_ptr<Target>> targets;

    models.push_back(std::make_shared<SpriteModel>());
    targets.push_back(std::make_shared<Sprite>());
    static_cast<Sprite *>(targets.back().get())->setInterface(static_cast<SpriteModel *>(models.back().get()));

    models.push_back(std::make_shared<StageModel>());
    targets.push_back(std::make_shared<Stage>());
    static_cast<Stage *>(targets.back().get())->setInterface(static_cast<StageModel *>(models.back().get()));

    for (int i = 0; i < targets.size(); i++) {
        auto target = targets[i];
        auto model = models[i];

        VirtualMachine vm(target.get(), &m_engineMock, nullptr);
        vm.setBytecode(bytecode1);
        vm.setFunctions(functions);
        vm.setConstValues(constValues);

        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().diameter, 512.5);

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().diameter, 1024);

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().diameter, 1200);

        vm.reset();
        vm.setBytecode(bytecode2);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().diameter, 549.92);

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().diameter, 1);
    }
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

    std::vector<std::shared_ptr<TargetModel>> models;
    std::vector<std::shared_ptr<Target>> targets;

    models.push_back(std::make_shared<SpriteModel>());
    targets.push_back(std::make_shared<Sprite>());
    static_cast<Sprite *>(targets.back().get())->setInterface(static_cast<SpriteModel *>(models.back().get()));

    models.push_back(std::make_shared<StageModel>());
    targets.push_back(std::make_shared<Stage>());
    static_cast<Stage *>(targets.back().get())->setInterface(static_cast<StageModel *>(models.back().get()));

    for (int i = 0; i < targets.size(); i++) {
        auto target = targets[i];
        auto model = models[i];

        VirtualMachine vm(target.get(), &m_engineMock, nullptr);
        vm.setBytecode(bytecode1);
        vm.setFunctions(functions);
        vm.setConstValues(constValues);

        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().diameter, 511.5);

        vm.reset();
        vm.setBytecode(bytecode2);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().diameter, 1);

        vm.reset();
        vm.setBytecode(bytecode3);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().diameter, 1200);
    }
}

TEST_F(PenBlocksTest, ChangePenShadeBy)
{
    Compiler compiler(&m_engineMock);

    // change pen shade by (4.5)
    auto block1 = std::make_shared<Block>("a", "pen_changePenShadeBy");
    addValueInput(block1, "SHADE", PenBlocks::SHADE, 4.5);

    // change pen shade by (null block)
    auto block2 = std::make_shared<Block>("b", "pen_changePenShadeBy");
    addObscuredInput(block2, "SHADE", PenBlocks::SHADE, createNullBlock("c"));

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::changePenShadeBy)).WillOnce(Return(2));
    compiler.setBlock(block1);
    PenBlocks::compileChangePenShadeBy(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::changePenShadeBy)).WillOnce(Return(2));
    compiler.setBlock(block2);
    PenBlocks::compileChangePenShadeBy(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 2, vm::OP_NULL, vm::OP_EXEC, 2, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 4.5);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(PenBlocksTest, ChangePenShadeByImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &PenBlocks::changePenShadeBy };
    static Value constValues[] = { 134.09, -124.45 };

    std::vector<std::shared_ptr<TargetModel>> models;
    std::vector<std::shared_ptr<Target>> targets;

    models.push_back(std::make_shared<SpriteModel>());
    targets.push_back(std::make_shared<Sprite>());
    static_cast<Sprite *>(targets.back().get())->setInterface(static_cast<SpriteModel *>(models.back().get()));

    models.push_back(std::make_shared<StageModel>());
    targets.push_back(std::make_shared<Stage>());
    static_cast<Stage *>(targets.back().get())->setInterface(static_cast<StageModel *>(models.back().get()));

    for (int i = 0; i < targets.size(); i++) {
        auto target = targets[i];
        auto model = models[i];
        model->penState().transparency = 100 * (1 - 150 / 255.0);

        VirtualMachine vm(target.get(), &m_engineMock, nullptr);
        vm.setBytecode(bytecode1);
        vm.setFunctions(functions);
        vm.setConstValues(constValues);

        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(240, 255, 110, 150)));

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(240, 119, 255, 150)));

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(240, 247, 255, 150)));

        vm.reset();
        vm.setBytecode(bytecode2);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(240, 162, 255, 150)));

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(240, 255, 55, 150)));
    }
}

TEST_F(PenBlocksTest, SetPenShadeToNumber)
{
    Compiler compiler(&m_engineMock);

    // set pen shade to (4.5)
    auto block1 = std::make_shared<Block>("a", "pen_setPenShadeToNumber");
    addValueInput(block1, "SHADE", PenBlocks::SHADE, 4.5);

    // set pen shade to (null block)
    auto block2 = std::make_shared<Block>("b", "pen_setPenShadeToNumber");
    addObscuredInput(block2, "SHADE", PenBlocks::SHADE, createNullBlock("c"));

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::setPenShadeToNumber)).WillOnce(Return(2));
    compiler.setBlock(block1);
    PenBlocks::compileSetPenShadeToNumber(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::setPenShadeToNumber)).WillOnce(Return(2));
    compiler.setBlock(block2);
    PenBlocks::compileSetPenShadeToNumber(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 2, vm::OP_NULL, vm::OP_EXEC, 2, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 4.5);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(PenBlocksTest, SetPenShadeToNumberImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &PenBlocks::setPenShadeToNumber };
    static Value constValues[] = { 125.7, -114.09, 489.4 };

    std::vector<std::shared_ptr<TargetModel>> models;
    std::vector<std::shared_ptr<Target>> targets;

    models.push_back(std::make_shared<SpriteModel>());
    targets.push_back(std::make_shared<Sprite>());
    static_cast<Sprite *>(targets.back().get())->setInterface(static_cast<SpriteModel *>(models.back().get()));

    models.push_back(std::make_shared<StageModel>());
    targets.push_back(std::make_shared<Stage>());
    static_cast<Stage *>(targets.back().get())->setInterface(static_cast<StageModel *>(models.back().get()));

    for (int i = 0; i < targets.size(); i++) {
        auto target = targets[i];
        auto model = models[i];
        model->penState().transparency = 100 * (1 - 150 / 255.0);

        VirtualMachine vm(target.get(), &m_engineMock, nullptr);
        vm.setBytecode(bytecode1);
        vm.setFunctions(functions);
        vm.setConstValues(constValues);

        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(240, 148, 253, 150)));

        vm.reset();
        vm.setBytecode(bytecode2);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(240, 102, 255, 150)));

        vm.reset();
        vm.setBytecode(bytecode3);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(240, 89, 255, 150)));
    }
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

    std::vector<std::shared_ptr<TargetModel>> models;
    std::vector<std::shared_ptr<Target>> targets;

    models.push_back(std::make_shared<SpriteModel>());
    targets.push_back(std::make_shared<Sprite>());
    static_cast<Sprite *>(targets.back().get())->setInterface(static_cast<SpriteModel *>(models.back().get()));

    models.push_back(std::make_shared<StageModel>());
    targets.push_back(std::make_shared<Stage>());
    static_cast<Stage *>(targets.back().get())->setInterface(static_cast<StageModel *>(models.back().get()));

    for (int i = 0; i < targets.size(); i++) {
        auto target = targets[i];
        auto model = models[i];
        model->penState().transparency = 100 * (1 - 150 / 255.0);

        VirtualMachine vm(target.get(), &m_engineMock, nullptr);
        vm.setBytecode(bytecode1);
        vm.setFunctions(functions);
        vm.setConstValues(constValues);

        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(106, 255, 255, 150)));

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(332, 255, 255, 150)));

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(199, 255, 255, 150)));

        vm.reset();
        vm.setBytecode(bytecode2);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(353, 255, 255, 150)));

        vm.reset();
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(148, 255, 255, 150)));
    }
}

TEST_F(PenBlocksTest, SetPenHueToNumber)
{
    Compiler compiler(&m_engineMock);

    // set pen hue to (54.09)
    auto block1 = std::make_shared<Block>("a", "pen_setPenHueToNumber");
    addValueInput(block1, "HUE", PenBlocks::HUE, 54.09);

    // set pen hue to (null block)
    auto block2 = std::make_shared<Block>("b", "pen_setPenHueToNumber");
    addObscuredInput(block2, "HUE", PenBlocks::HUE, createNullBlock("c"));

    compiler.init();

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::setPenHueToNumber)).WillOnce(Return(2));
    compiler.setBlock(block1);
    PenBlocks::compileSetPenHueToNumber(&compiler);

    EXPECT_CALL(m_engineMock, functionIndex(&PenBlocks::setPenHueToNumber)).WillOnce(Return(2));
    compiler.setBlock(block2);
    PenBlocks::compileSetPenHueToNumber(&compiler);

    compiler.end();

    ASSERT_EQ(compiler.bytecode(), std::vector<unsigned int>({ vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 2, vm::OP_NULL, vm::OP_EXEC, 2, vm::OP_HALT }));
    ASSERT_EQ(compiler.constValues().size(), 1);
    ASSERT_EQ(compiler.constValues()[0].toDouble(), 54.09);
    ASSERT_TRUE(compiler.variables().empty());
    ASSERT_TRUE(compiler.lists().empty());
}

TEST_F(PenBlocksTest, SetPenHueToNumberImpl)
{
    static unsigned int bytecode1[] = { vm::OP_START, vm::OP_CONST, 0, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode2[] = { vm::OP_START, vm::OP_CONST, 1, vm::OP_EXEC, 0, vm::OP_HALT };
    static unsigned int bytecode3[] = { vm::OP_START, vm::OP_CONST, 2, vm::OP_EXEC, 0, vm::OP_HALT };
    static BlockFunc functions[] = { &PenBlocks::setPenHueToNumber };
    static Value constValues[] = { 125.7, -114.09, 489.4 };

    std::vector<std::shared_ptr<TargetModel>> models;
    std::vector<std::shared_ptr<Target>> targets;

    models.push_back(std::make_shared<SpriteModel>());
    targets.push_back(std::make_shared<Sprite>());
    static_cast<Sprite *>(targets.back().get())->setInterface(static_cast<SpriteModel *>(models.back().get()));

    models.push_back(std::make_shared<StageModel>());
    targets.push_back(std::make_shared<Stage>());
    static_cast<Stage *>(targets.back().get())->setInterface(static_cast<StageModel *>(models.back().get()));

    for (int i = 0; i < targets.size(); i++) {
        auto target = targets[i];
        auto model = models[i];
        model->penState().transparency = 100 * (1 - 150 / 255.0);

        VirtualMachine vm(target.get(), &m_engineMock, nullptr);
        vm.setBytecode(bytecode1);
        vm.setFunctions(functions);
        vm.setConstValues(constValues);

        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(226, 255, 255, 255)));

        vm.reset();
        vm.setBytecode(bytecode2);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(154, 255, 255, 255)));

        vm.reset();
        vm.setBytecode(bytecode3);
        vm.run();
        ASSERT_EQ(vm.registerCount(), 0);
        ASSERT_EQ(model->penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(160, 255, 255, 255)));
    }
}
