#include <projectscene.h>
#include <enginemock.h>

#include "../common.h"

using namespace scratchcppgui;

using ::testing::Return;

TEST(ProjectScene, Engine)
{
    ProjectScene scene;
    ASSERT_EQ(scene.engine(), nullptr);

    EngineMock engine;
    scene.setEngine(&engine);
    ASSERT_EQ(scene.engine(), &engine);
}
