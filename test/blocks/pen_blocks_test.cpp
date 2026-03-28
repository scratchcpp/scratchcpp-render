#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/stage.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/test/scriptbuilder.h>
#include <enginemock.h>
#include <penlayermock.h>
#include <gtest/gtest.h>

#include "penlayer.h"
#include "spritemodel.h"
#include "stagemodel.h"
#include "renderedtarget.h"
#include "blocks/penblocks.h"
#include "util.h"

using namespace scratchcpprender;
using namespace libscratchcpp;
using namespace libscratchcpp::test;

using ::testing::Return;
using ::testing::ReturnRef;

class PenBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension = std::make_unique<PenBlocks>();
            m_engine = m_project.engine().get();
            m_extension->registerBlocks(m_engine);
            registerBlocks(m_engine, m_extension.get());

            PenLayer::addPenLayer(&m_engineMock, &m_penLayer);

            EXPECT_CALL(m_engineMock, targets()).WillRepeatedly(ReturnRef(m_engine->targets()));
        }

        void TearDown() override { PenLayer::removePenLayer(&m_engineMock); }

        std::shared_ptr<Thread> buildScript(ScriptBuilder &builder, Target *target)
        {
            auto block = builder.currentBlock();

            m_compiler = std::make_unique<Compiler>(&m_engineMock, target);
            auto code = m_compiler->compile(block);
            m_script = std::make_unique<Script>(target, block, &m_engineMock);
            m_script->setCode(code);
            return std::make_shared<Thread>(target, &m_engineMock, m_script.get());
        }

        std::unique_ptr<IExtension> m_extension;
        Project m_project;
        IEngine *m_engine = nullptr;
        EngineMock m_engineMock;
        PenLayerMock m_penLayer;

    private:
        std::unique_ptr<Compiler> m_compiler;
        std::unique_ptr<Script> m_script;
};

TEST_F(PenBlocksTest, Clear)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_clear");

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_penLayer, clear());
    EXPECT_CALL(m_engineMock, requestRedraw());
    thread->run();
}

TEST_F(PenBlocksTest, Stamp_Sprite)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_stamp");

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_penLayer, stamp(&renderedTarget));
    EXPECT_CALL(m_engineMock, requestRedraw());
    thread->run();
}

TEST_F(PenBlocksTest, Stamp_Stage)
{
    auto stage = std::make_shared<Stage>();
    stage->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    StageModel model;
    model.init(stage.get());
    model.setRenderedTarget(&renderedTarget);
    stage->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("pen_stamp");

    auto thread = buildScript(builder, stage.get());

    EXPECT_CALL(m_penLayer, stamp(&renderedTarget));
    EXPECT_CALL(m_engineMock, requestRedraw());
    thread->run();
}

TEST_F(PenBlocksTest, PenDown_Sprite)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_penDown");

    auto thread = buildScript(builder, sprite.get());

    model.setPenDown(false);

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();
    ASSERT_TRUE(model.penDown());

    thread->run();
    ASSERT_TRUE(model.penDown());
}

TEST_F(PenBlocksTest, PenDown_Stage)
{
    auto stage = std::make_shared<Stage>();
    stage->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    StageModel model;
    model.init(stage.get());
    model.setRenderedTarget(&renderedTarget);
    stage->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("pen_penDown");

    auto thread = buildScript(builder, stage.get());

    model.setPenDown(false);

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();
    ASSERT_TRUE(model.penDown());

    thread->run();
    ASSERT_TRUE(model.penDown());
}

TEST_F(PenBlocksTest, PenUp_Sprite)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_penUp");

    auto thread = buildScript(builder, sprite.get());

    model.setPenDown(true);

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();
    ASSERT_FALSE(model.penDown());

    thread->run();
    ASSERT_FALSE(model.penDown());
}

TEST_F(PenBlocksTest, PenUp_Stage)
{
    auto stage = std::make_shared<Stage>();
    stage->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    StageModel model;
    model.init(stage.get());
    model.setRenderedTarget(&renderedTarget);
    stage->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("pen_penUp");

    auto thread = buildScript(builder, stage.get());

    model.setPenDown(true);

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();
    ASSERT_FALSE(model.penDown());

    thread->run();
    ASSERT_FALSE(model.penDown());
}

TEST_F(PenBlocksTest, SetPenColorToColor_ValidHex)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorToColor");
    builder.addValueInput("COLOR", "#FFGFFF");

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();
    ASSERT_EQ(model.penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(359, 0, 0)));
}

TEST_F(PenBlocksTest, SetPenColorToColor_NonConstHex)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "#FFGFFF");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("pen_setPenColorToColor");
    builder.addObscuredInput("COLOR", valueBlock);

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();
    ASSERT_EQ(model.penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(359, 0, 0)));
}

TEST_F(PenBlocksTest, SetPenColorToColor_CaseInsensitiveHex)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorToColor");
    builder.addValueInput("COLOR", "#AABbCC");

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();
    ASSERT_EQ(model.penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(210, 42, 204)));
}

TEST_F(PenBlocksTest, SetPenColorToColor_3DigitHex)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorToColor");
    builder.addValueInput("COLOR", "#03F");

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();
    ASSERT_EQ(model.penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(228, 255, 255)));
}

TEST_F(PenBlocksTest, SetPenColorToColor_InvalidHex)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorToColor");
    builder.addValueInput("COLOR", "#AABBCCDD");

    model.penAttributes().color = QNanoColor::fromQColor(QColor::fromHsv(228, 255, 255));

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    ASSERT_EQ(model.penAttributes().color, QNanoColor::fromQColor(QColor(0, 0, 0)));
}

TEST_F(PenBlocksTest, SetPenColorToColor_InvalidString)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorToColor");
    builder.addValueInput("COLOR", "FFFFFF");

    model.penAttributes().color = QNanoColor::fromQColor(QColor::fromHsv(228, 255, 255));

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    ASSERT_EQ(model.penAttributes().color, QNanoColor::fromQColor(QColor(0, 0, 0)));
}

TEST_F(PenBlocksTest, SetPenColorToColor_NumberWithoutAlphaChannel)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorToColor");
    builder.addValueInput("COLOR", 255);

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    ASSERT_EQ(model.penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(239, 255, 255)));
}

TEST_F(PenBlocksTest, SetPenColorToColor_StringNumber)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorToColor");
    builder.addValueInput("COLOR", "255");

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    ASSERT_EQ(model.penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(239, 255, 255)));
}

TEST_F(PenBlocksTest, SetPenColorToColor_NumberWithAlphaChannel)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorToColor");
    builder.addValueInput("COLOR", 1228097602);

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    ASSERT_EQ(model.penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(162, 74, 72, 73)));
}

TEST_F(PenBlocksTest, SetPenColorToColor_Stage)
{
    auto stage = std::make_shared<Stage>();
    stage->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    StageModel model;
    model.init(stage.get());
    model.setRenderedTarget(&renderedTarget);
    stage->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("pen_setPenColorToColor");
    builder.addValueInput("COLOR", rgb(128, 255, 26));

    auto thread = buildScript(builder, stage.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    ASSERT_EQ(model.penAttributes().color, QNanoColor::fromQColor(QColor::fromHsv(93, 229, 255)));
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_Color)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenColorParamBy");
    builder.addDropdownInput("COLOR_PARAM", "color");
    builder.addValueInput("VALUE", 53.2);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 91);
    EXPECT_EQ(model.penAttributes().color.green(), 94);
    EXPECT_EQ(model.penAttributes().color.blue(), 149);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_Color_OutOfRange)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenColorParamBy");
    builder.addDropdownInput("COLOR_PARAM", "color");
    builder.addValueInput("VALUE", 153.2);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 91);
    EXPECT_EQ(model.penAttributes().color.green(), 94);
    EXPECT_EQ(model.penAttributes().color.blue(), 149);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_Color_OutOfRange_Negative)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenColorParamBy");
    builder.addDropdownInput("COLOR_PARAM", "color");
    builder.addValueInput("VALUE", -120.8);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 149);
    EXPECT_EQ(model.penAttributes().color.green(), 91);
    EXPECT_EQ(model.penAttributes().color.blue(), 120);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_NonConstParam_Color)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "color");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("pen_changePenColorParamBy");
    builder.addObscuredInput("COLOR_PARAM", valueBlock);
    builder.addValueInput("VALUE", 53.2);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 91);
    EXPECT_EQ(model.penAttributes().color.green(), 94);
    EXPECT_EQ(model.penAttributes().color.blue(), 149);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_ParamNameCaseSensitive)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenColorParamBy");
    builder.addDropdownInput("COLOR_PARAM", "coLor");
    builder.addValueInput("VALUE", 53.2);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    // The color shouldn't change
    ASSERT_EQ(model.penAttributes().color, original);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_NonConstParamNameCaseInsensitive)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "coLor");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("pen_changePenColorParamBy");
    builder.addObscuredInput("COLOR_PARAM", valueBlock);
    builder.addValueInput("VALUE", 53.2);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    // The color shouldn't change
    ASSERT_EQ(model.penAttributes().color, original);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_Saturation)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenColorParamBy");
    builder.addDropdownInput("COLOR_PARAM", "saturation");
    builder.addValueInput("VALUE", 23.5);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 149);
    EXPECT_EQ(model.penAttributes().color.green(), 126);
    EXPECT_EQ(model.penAttributes().color.blue(), 56);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_Saturation_OutOfRange)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenColorParamBy");
    builder.addDropdownInput("COLOR_PARAM", "saturation");
    builder.addValueInput("VALUE", 138.3);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 149);
    EXPECT_EQ(model.penAttributes().color.green(), 112);
    EXPECT_EQ(model.penAttributes().color.blue(), 0);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_Saturation_OutOfRange_Negative)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenColorParamBy");
    builder.addDropdownInput("COLOR_PARAM", "saturation");
    builder.addValueInput("VALUE", -120.8);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 149);
    EXPECT_EQ(model.penAttributes().color.green(), 149);
    EXPECT_EQ(model.penAttributes().color.blue(), 149);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_NonConstParam_Saturation)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "saturation");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("pen_changePenColorParamBy");
    builder.addObscuredInput("COLOR_PARAM", valueBlock);
    builder.addValueInput("VALUE", 23.5);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 149);
    EXPECT_EQ(model.penAttributes().color.green(), 126);
    EXPECT_EQ(model.penAttributes().color.blue(), 56);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_Brightness)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenColorParamBy");
    builder.addDropdownInput("COLOR_PARAM", "brightness");
    builder.addValueInput("VALUE", 23.5);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 209);
    EXPECT_EQ(model.penAttributes().color.green(), 189);
    EXPECT_EQ(model.penAttributes().color.blue(), 128);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_Brightness_OutOfRange)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenColorParamBy");
    builder.addDropdownInput("COLOR_PARAM", "brightness");
    builder.addValueInput("VALUE", 138.3);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 255);
    EXPECT_EQ(model.penAttributes().color.green(), 230);
    EXPECT_EQ(model.penAttributes().color.blue(), 156);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_Brightness_OutOfRange_Negative)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenColorParamBy");
    builder.addDropdownInput("COLOR_PARAM", "brightness");
    builder.addValueInput("VALUE", -120.8);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 0);
    EXPECT_EQ(model.penAttributes().color.green(), 0);
    EXPECT_EQ(model.penAttributes().color.blue(), 0);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_NonConstParam_Brightness)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "brightness");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("pen_changePenColorParamBy");
    builder.addObscuredInput("COLOR_PARAM", valueBlock);
    builder.addValueInput("VALUE", 23.5);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 209);
    EXPECT_EQ(model.penAttributes().color.green(), 189);
    EXPECT_EQ(model.penAttributes().color.blue(), 128);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_Transparency)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenColorParamBy");
    builder.addDropdownInput("COLOR_PARAM", "transparency");
    builder.addValueInput("VALUE", 23.5);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 149);
    EXPECT_EQ(model.penAttributes().color.green(), 135);
    EXPECT_EQ(model.penAttributes().color.blue(), 91);
    EXPECT_EQ(model.penAttributes().color.alpha(), 195);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_Transparency_OutOfRange)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenColorParamBy");
    builder.addDropdownInput("COLOR_PARAM", "transparency");
    builder.addValueInput("VALUE", 138.3);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.alpha(), 0);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_Transparency_OutOfRange_Negative)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenColorParamBy");
    builder.addDropdownInput("COLOR_PARAM", "transparency");
    builder.addValueInput("VALUE", -120.8);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 149);
    EXPECT_EQ(model.penAttributes().color.green(), 135);
    EXPECT_EQ(model.penAttributes().color.blue(), 91);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_NonConstParam_Transparency)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "transparency");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("pen_changePenColorParamBy");
    builder.addObscuredInput("COLOR_PARAM", valueBlock);
    builder.addValueInput("VALUE", 23.5);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 149);
    EXPECT_EQ(model.penAttributes().color.green(), 135);
    EXPECT_EQ(model.penAttributes().color.blue(), 91);
    EXPECT_EQ(model.penAttributes().color.alpha(), 195);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_InvalidParam)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenColorParamBy");
    builder.addDropdownInput("COLOR_PARAM", "invalid");
    builder.addValueInput("VALUE", 53.2);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    // The color shouldn't change
    ASSERT_EQ(model.penAttributes().color, original);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_NonConstInvalidParam)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "test");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("pen_changePenColorParamBy");
    builder.addObscuredInput("COLOR_PARAM", valueBlock);
    builder.addValueInput("VALUE", 53.2);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    // The color shouldn't change
    ASSERT_EQ(model.penAttributes().color, original);
}

TEST_F(PenBlocksTest, ChangePenColorParamBy_Stage)
{
    auto stage = std::make_shared<Stage>();
    stage->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    StageModel model;
    model.init(stage.get());
    model.setRenderedTarget(&renderedTarget);
    stage->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("pen_changePenColorParamBy");
    builder.addDropdownInput("COLOR_PARAM", "color");
    builder.addValueInput("VALUE", 53.2);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, stage.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 91);
    EXPECT_EQ(model.penAttributes().color.green(), 94);
    EXPECT_EQ(model.penAttributes().color.blue(), 149);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_Color)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorParamTo");
    builder.addDropdownInput("COLOR_PARAM", "color");
    builder.addValueInput("VALUE", 53.2);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 91);
    EXPECT_EQ(model.penAttributes().color.green(), 138);
    EXPECT_EQ(model.penAttributes().color.blue(), 149);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_Color_OutOfRange)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorParamTo");
    builder.addDropdownInput("COLOR_PARAM", "color");
    builder.addValueInput("VALUE", 153.2);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 91);
    EXPECT_EQ(model.penAttributes().color.green(), 138);
    EXPECT_EQ(model.penAttributes().color.blue(), 149);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_Color_OutOfRange_Negative)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorParamTo");
    builder.addDropdownInput("COLOR_PARAM", "color");
    builder.addValueInput("VALUE", -120.8);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 135);
    EXPECT_EQ(model.penAttributes().color.green(), 91);
    EXPECT_EQ(model.penAttributes().color.blue(), 149);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_NonConstParam_Color)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "color");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("pen_setPenColorParamTo");
    builder.addObscuredInput("COLOR_PARAM", valueBlock);
    builder.addValueInput("VALUE", 53.2);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 91);
    EXPECT_EQ(model.penAttributes().color.green(), 138);
    EXPECT_EQ(model.penAttributes().color.blue(), 149);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_ParamNameCaseSensitive)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorParamTo");
    builder.addDropdownInput("COLOR_PARAM", "coLor");
    builder.addValueInput("VALUE", 53.2);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    // The color shouldn't change
    ASSERT_EQ(model.penAttributes().color, original);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_NonConstParamNameCaseInsensitive)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "coLor");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("pen_setPenColorParamTo");
    builder.addObscuredInput("COLOR_PARAM", valueBlock);
    builder.addValueInput("VALUE", 53.2);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    // The color shouldn't change
    ASSERT_EQ(model.penAttributes().color, original);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_Saturation)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorParamTo");
    builder.addDropdownInput("COLOR_PARAM", "saturation");
    builder.addValueInput("VALUE", 23.5);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 149);
    EXPECT_EQ(model.penAttributes().color.green(), 140);
    EXPECT_EQ(model.penAttributes().color.blue(), 115);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_Saturation_OutOfRange)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorParamTo");
    builder.addDropdownInput("COLOR_PARAM", "saturation");
    builder.addValueInput("VALUE", 138.3);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 149);
    EXPECT_EQ(model.penAttributes().color.green(), 112);
    EXPECT_EQ(model.penAttributes().color.blue(), 0);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_Saturation_OutOfRange_Negative)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorParamTo");
    builder.addDropdownInput("COLOR_PARAM", "saturation");
    builder.addValueInput("VALUE", -120.8);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 149);
    EXPECT_EQ(model.penAttributes().color.green(), 149);
    EXPECT_EQ(model.penAttributes().color.blue(), 149);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_NonConstParam_Saturation)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "saturation");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("pen_setPenColorParamTo");
    builder.addObscuredInput("COLOR_PARAM", valueBlock);
    builder.addValueInput("VALUE", 23.5);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 149);
    EXPECT_EQ(model.penAttributes().color.green(), 140);
    EXPECT_EQ(model.penAttributes().color.blue(), 115);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_Brightness)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorParamTo");
    builder.addDropdownInput("COLOR_PARAM", "brightness");
    builder.addValueInput("VALUE", 23.5);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 59);
    EXPECT_EQ(model.penAttributes().color.green(), 53);
    EXPECT_EQ(model.penAttributes().color.blue(), 36);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_Brightness_OutOfRange)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorParamTo");
    builder.addDropdownInput("COLOR_PARAM", "brightness");
    builder.addValueInput("VALUE", 138.3);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 255);
    EXPECT_EQ(model.penAttributes().color.green(), 230);
    EXPECT_EQ(model.penAttributes().color.blue(), 156);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_Brightness_OutOfRange_Negative)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorParamTo");
    builder.addDropdownInput("COLOR_PARAM", "brightness");
    builder.addValueInput("VALUE", -120.8);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 0);
    EXPECT_EQ(model.penAttributes().color.green(), 0);
    EXPECT_EQ(model.penAttributes().color.blue(), 0);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_NonConstParam_Brightness)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "brightness");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("pen_setPenColorParamTo");
    builder.addObscuredInput("COLOR_PARAM", valueBlock);
    builder.addValueInput("VALUE", 23.5);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 59);
    EXPECT_EQ(model.penAttributes().color.green(), 53);
    EXPECT_EQ(model.penAttributes().color.blue(), 36);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_Transparency)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorParamTo");
    builder.addDropdownInput("COLOR_PARAM", "transparency");
    builder.addValueInput("VALUE", 23.5);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 149);
    EXPECT_EQ(model.penAttributes().color.green(), 135);
    EXPECT_EQ(model.penAttributes().color.blue(), 91);
    EXPECT_EQ(model.penAttributes().color.alpha(), 195);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_Transparency_OutOfRange)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorParamTo");
    builder.addDropdownInput("COLOR_PARAM", "transparency");
    builder.addValueInput("VALUE", 138.3);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.alpha(), 0);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_Transparency_OutOfRange_Negative)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorParamTo");
    builder.addDropdownInput("COLOR_PARAM", "transparency");
    builder.addValueInput("VALUE", -120.8);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 149);
    EXPECT_EQ(model.penAttributes().color.green(), 135);
    EXPECT_EQ(model.penAttributes().color.blue(), 91);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_NonConstParam_Transparency)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "transparency");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("pen_setPenColorParamTo");
    builder.addObscuredInput("COLOR_PARAM", valueBlock);
    builder.addValueInput("VALUE", 23.5);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 149);
    EXPECT_EQ(model.penAttributes().color.green(), 135);
    EXPECT_EQ(model.penAttributes().color.blue(), 91);
    EXPECT_EQ(model.penAttributes().color.alpha(), 195);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_InvalidParam)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenColorParamTo");
    builder.addDropdownInput("COLOR_PARAM", "invalid");
    builder.addValueInput("VALUE", 53.2);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    // The color shouldn't change
    ASSERT_EQ(model.penAttributes().color, original);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_NonConstInvalidParam)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("test_const_string");
    builder.addValueInput("STRING", "test");
    auto valueBlock = builder.takeBlock();

    builder.addBlock("pen_setPenColorParamTo");
    builder.addObscuredInput("COLOR_PARAM", valueBlock);
    builder.addValueInput("VALUE", 53.2);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    // The color shouldn't change
    ASSERT_EQ(model.penAttributes().color, original);
}

TEST_F(PenBlocksTest, SetPenColorParamTo_Stage)
{
    auto stage = std::make_shared<Stage>();
    stage->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    StageModel model;
    model.init(stage.get());
    model.setRenderedTarget(&renderedTarget);
    stage->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("pen_setPenColorParamTo");
    builder.addDropdownInput("COLOR_PARAM", "color");
    builder.addValueInput("VALUE", 53.2);

    PenState &penState = model.penState();
    penState.color = 12.67;
    penState.saturation = 39.21;
    penState.brightness = 58.82;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, stage.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 91);
    EXPECT_EQ(model.penAttributes().color.green(), 138);
    EXPECT_EQ(model.penAttributes().color.blue(), 149);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
}

TEST_F(PenBlocksTest, ChangePenSizeBy)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenSizeBy");
    builder.addValueInput("SIZE", 511.5);

    model.penAttributes().diameter = 2.3;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);

    thread->run();
    ASSERT_EQ(model.penAttributes().diameter, 513.8);
}

TEST_F(PenBlocksTest, ChangePenSizeBy_Negative)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenSizeBy");
    builder.addValueInput("SIZE", -0.5);

    model.penAttributes().diameter = 2.3;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);

    thread->run();
    ASSERT_EQ(std::round(model.penAttributes().diameter * 100) / 100, 1.8);
}

TEST_F(PenBlocksTest, ChangePenSizeBy_AboveMaximum)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenSizeBy");
    builder.addValueInput("SIZE", 687.2);

    model.penAttributes().diameter = 513.8;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);

    thread->run();
    ASSERT_EQ(model.penAttributes().diameter, 1200);
}

TEST_F(PenBlocksTest, ChangePenSizeBy_BelowMinimum)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenSizeBy");
    builder.addValueInput("SIZE", -13);

    model.penAttributes().diameter = 12.5;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);

    thread->run();
    ASSERT_EQ(model.penAttributes().diameter, 1);
}

TEST_F(PenBlocksTest, ChangePenSizeBy_Stage)
{
    auto stage = std::make_shared<Stage>();
    stage->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    StageModel model;
    model.init(stage.get());
    model.setRenderedTarget(&renderedTarget);
    stage->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("pen_changePenSizeBy");
    builder.addValueInput("SIZE", 511.5);

    model.penAttributes().diameter = 2.3;

    auto thread = buildScript(builder, stage.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);

    thread->run();
    ASSERT_EQ(model.penAttributes().diameter, 513.8);
}

TEST_F(PenBlocksTest, SetPenSizeTo)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenSizeTo");
    builder.addValueInput("SIZE", 511.5);

    model.penAttributes().diameter = 2.3;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);

    thread->run();
    ASSERT_EQ(model.penAttributes().diameter, 511.5);
}

TEST_F(PenBlocksTest, SetPenSizeTo_Negative)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenSizeTo");
    builder.addValueInput("SIZE", -0.5);

    model.penAttributes().diameter = 2.3;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);

    thread->run();
    ASSERT_EQ(model.penAttributes().diameter, 1);
}

TEST_F(PenBlocksTest, SetPenSizeTo_AboveMaximum)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenSizeTo");
    builder.addValueInput("SIZE", 1200.6);

    model.penAttributes().diameter = 513.8;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);

    thread->run();
    ASSERT_EQ(model.penAttributes().diameter, 1200);
}

TEST_F(PenBlocksTest, SetPenSizeTo_BelowMinimum)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_setPenSizeTo");
    builder.addValueInput("SIZE", 0.5);

    model.penAttributes().diameter = 12.5;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);

    thread->run();
    ASSERT_EQ(model.penAttributes().diameter, 1);
}

TEST_F(PenBlocksTest, SetPenSizeTo_Stage)
{
    auto stage = std::make_shared<Stage>();
    stage->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    StageModel model;
    model.init(stage.get());
    model.setRenderedTarget(&renderedTarget);
    stage->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("pen_setPenSizeTo");
    builder.addValueInput("SIZE", 511.5);

    model.penAttributes().diameter = 2.3;

    auto thread = buildScript(builder, stage.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);

    thread->run();
    ASSERT_EQ(model.penAttributes().diameter, 511.5);
}

TEST_F(PenBlocksTest, ChangePenShadeBy)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenShadeBy");
    builder.addValueInput("SHADE", 49.2);

    PenState &penState = model.penState();
    penState.color = 60;
    penState.saturation = 90;
    penState.brightness = 75;
    penState.shade = 13.8;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 55);
    EXPECT_EQ(model.penAttributes().color.green(), 135);
    EXPECT_EQ(model.penAttributes().color.blue(), 255);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
    EXPECT_EQ(model.penState().shade, 63);
}

TEST_F(PenBlocksTest, ChangePenShadeBy_OutOfRange)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenShadeBy");
    builder.addValueInput("SHADE", 189.6);

    PenState &penState = model.penState();
    penState.color = 60;
    penState.saturation = 90;
    penState.brightness = 75;
    penState.shade = 13.8;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 0);
    EXPECT_EQ(model.penAttributes().color.green(), 22);
    EXPECT_EQ(model.penAttributes().color.blue(), 56);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
    EXPECT_EQ(std::round(model.penState().shade * 100) / 100, 3.4);
}

TEST_F(PenBlocksTest, ChangePenShadeBy_OutOfRange_Negative)
{
    auto sprite = std::make_shared<Sprite>();
    sprite->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    SpriteModel model;
    model.init(sprite.get());
    model.setRenderedTarget(&renderedTarget);
    sprite->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, sprite);
    builder.addBlock("pen_changePenShadeBy");
    builder.addValueInput("SHADE", -25.3);

    PenState &penState = model.penState();
    penState.color = 60;
    penState.saturation = 90;
    penState.brightness = 75;
    penState.shade = 13.8;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, sprite.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 0);
    EXPECT_EQ(model.penAttributes().color.green(), 36);
    EXPECT_EQ(model.penAttributes().color.blue(), 91);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
    EXPECT_EQ(model.penState().shade, 188.5);
}

TEST_F(PenBlocksTest, ChangePenShadeBy_Stage)
{
    auto stage = std::make_shared<Stage>();
    stage->setEngine(&m_engineMock);

    RenderedTarget renderedTarget;
    StageModel model;
    model.init(stage.get());
    model.setRenderedTarget(&renderedTarget);
    stage->setInterface(&model);

    ScriptBuilder builder(m_extension.get(), m_engine, stage);
    builder.addBlock("pen_changePenShadeBy");
    builder.addValueInput("SHADE", 49.2);

    PenState &penState = model.penState();
    penState.color = 60;
    penState.saturation = 90;
    penState.brightness = 75;
    penState.shade = 13.8;
    penState.updateColor();

    auto original = model.penAttributes().color;

    auto thread = buildScript(builder, stage.get());

    EXPECT_CALL(m_engineMock, requestRedraw).Times(0);
    thread->run();

    EXPECT_EQ(model.penAttributes().color.red(), 55);
    EXPECT_EQ(model.penAttributes().color.green(), 135);
    EXPECT_EQ(model.penAttributes().color.blue(), 255);
    EXPECT_EQ(model.penAttributes().color.alpha(), 255);
    EXPECT_EQ(model.penState().shade, 63);
}
