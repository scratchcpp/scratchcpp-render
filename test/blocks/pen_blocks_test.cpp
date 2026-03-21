#include <scratchcpp/project.h>
#include <enginemock.h>
#include <gtest/gtest.h>

#include "blocks/penblocks.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

class PenBlocksTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_extension = std::make_unique<PenBlocks>();
            m_engine = m_project.engine().get();
            m_extension->registerBlocks(m_engine);
        }

        std::unique_ptr<IExtension> m_extension;
        Project m_project;
        IEngine *m_engine = nullptr;
        EngineMock m_engineMock;
};
