#include <scratchcpp/project.h>
#include <scratchcpp/sprite.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/script.h>
#include <scratchcpp/thread.h>
#include <scratchcpp/test/scriptbuilder.h>
#include <enginemock.h>
#include <penlayermock.h>
#include <gtest/gtest.h>

#include "penlayer.h"
#include "blocks/penblocks.h"

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
