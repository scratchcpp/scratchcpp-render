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

TEST(GraphicsEffectTest, Clamp)
{
    {
        GraphicsEffect effect(ShaderManager::Effect::Color, "color");
        ASSERT_EQ(effect.clamp(-500), -500);
        ASSERT_EQ(effect.clamp(0), 0);
        ASSERT_EQ(effect.clamp(500), 500);
    }

    {
        GraphicsEffect effect(ShaderManager::Effect::Brightness, "brightness");
        ASSERT_EQ(effect.clamp(-125), -100);
        ASSERT_EQ(effect.clamp(-100), -100);
        ASSERT_EQ(effect.clamp(0), 0);
        ASSERT_EQ(effect.clamp(100), 100);
        ASSERT_EQ(effect.clamp(125), 100);
    }

    {
        GraphicsEffect effect(ShaderManager::Effect::Ghost, "ghost");
        ASSERT_EQ(effect.clamp(-50), 0);
        ASSERT_EQ(effect.clamp(0), 0);
        ASSERT_EQ(effect.clamp(100), 100);
        ASSERT_EQ(effect.clamp(125), 100);
    }

    // TODO: Test remaining effects
}
