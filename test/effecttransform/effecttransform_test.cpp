#include <QVector2D>
#include <effecttransform.h>

#include "../common.h"

using namespace scratchcpprender;

class EffectTransformTest : public testing::Test
{
    public:
        void SetUp() override { }

        std::unordered_map<ShaderManager::Effect, double> m_effects;
};

TEST_F(EffectTransformTest, NoEffect)
{
    QRgb color = qRgba(0, 0, 0, 0);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), color);

    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), color);

    color = qRgba(0, 255, 255, 255);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), color);

    color = qRgba(255, 255, 255, 128);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), color);

    QVector2D dst;
    EffectTransform::transformPoint(m_effects, QVector2D(0.5, -0.3), dst);
    ASSERT_EQ(dst, QVector2D(0.5, -0.3));
}

TEST_F(EffectTransformTest, ColorEffect)
{
    // 100
    m_effects[ShaderManager::Effect::Color] = 100;
    QRgb color = qRgba(0, 0, 0, 0);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), color);

    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgb(0, 255, 255));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgba(128, 100, 100, 128));

    // 175
    m_effects[ShaderManager::Effect::Color] = 175;
    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgb(255, 0, 191));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgba(100, 128, 107, 128));
}

TEST_F(EffectTransformTest, BrightnessEffect)
{
    // -100
    m_effects[ShaderManager::Effect::Brightness] = -100;
    QRgb color = qRgba(0, 0, 0, 0);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), color);

    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgb(0, 0, 0));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgba(0, 0, 0, 128));

    // -50
    m_effects[ShaderManager::Effect::Brightness] = -50;
    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgb(127, 0, 0));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgba(36, 64, 64, 128));

    // 50
    m_effects[ShaderManager::Effect::Brightness] = 50;
    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgb(255, 127, 127));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgba(128, 128, 128, 128));

    // 100
    m_effects[ShaderManager::Effect::Brightness] = 100;
    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgb(255, 255, 255));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgba(128, 128, 128, 128));
}

TEST_F(EffectTransformTest, GhostEffect)
{
    // 25
    m_effects[ShaderManager::Effect::Ghost] = 25;
    QRgb color = qRgba(0, 0, 0, 0);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), color);

    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgba(191, 0, 0, 191));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgba(75, 191, 150, 96));

    // 50
    m_effects[ShaderManager::Effect::Ghost] = 50;
    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgba(128, 0, 0, 128));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgba(50, 128, 100, 64));

    // 100
    m_effects[ShaderManager::Effect::Ghost] = 100;
    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgba(0, 0, 0, 0));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(m_effects, color), qRgba(0, 0, 0, 0));
}
