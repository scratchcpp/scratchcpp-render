#include <graphicseffect.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

TEST(GraphicsEffectTest, Constructor)
{
    {
        GraphicsEffect effect(ShaderManager::Effect::Color, "color");
        ASSERT_EQ(effect.effect(), ShaderManager::Effect::Color);
        ASSERT_EQ(effect.name(), "color");

        IGraphicsEffect *iface = dynamic_cast<IGraphicsEffect *>(&effect);
        ASSERT_TRUE(iface);
        ASSERT_EQ(iface->name(), "color");
    }

    {
        GraphicsEffect effect(ShaderManager::Effect::Brightness, "brightness");
        ASSERT_EQ(effect.effect(), ShaderManager::Effect::Brightness);
        ASSERT_EQ(effect.name(), "brightness");

        IGraphicsEffect *iface = dynamic_cast<IGraphicsEffect *>(&effect);
        ASSERT_TRUE(iface);
        ASSERT_EQ(iface->name(), "brightness");
    }
}
