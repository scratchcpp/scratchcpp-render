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
    auto mask = ShaderManager::Effect::NoEffect;
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), color);

    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), color);

    color = qRgba(0, 255, 255, 255);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), color);

    color = qRgba(255, 255, 255, 128);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), color);

    QVector2D dst;
    EffectTransform::transformPoint(mask, m_effects, QSize(), QVector2D(0.5, -0.3), dst);
    ASSERT_EQ(dst, QVector2D(0.5, -0.3));
}

TEST_F(EffectTransformTest, ColorEffect)
{
    // 100
    m_effects[ShaderManager::Effect::Color] = 100;
    auto mask = ShaderManager::Effect::Color;
    QRgb color = qRgba(0, 0, 0, 0);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), color);

    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgb(0, 255, 255));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgba(128, 100, 100, 128));

    // 175
    m_effects[ShaderManager::Effect::Color] = 175;
    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgb(255, 0, 191));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgba(100, 128, 107, 128));
}

TEST_F(EffectTransformTest, BrightnessEffect)
{
    // -100
    m_effects[ShaderManager::Effect::Brightness] = -100;
    auto mask = ShaderManager::Effect::Brightness;
    QRgb color = qRgba(0, 0, 0, 0);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), color);

    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgb(0, 0, 0));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgba(0, 0, 0, 128));

    // -50
    m_effects[ShaderManager::Effect::Brightness] = -50;
    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgb(127, 0, 0));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgba(36, 64, 64, 128));

    // 50
    m_effects[ShaderManager::Effect::Brightness] = 50;
    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgb(255, 127, 127));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgba(128, 128, 128, 128));

    // 100
    m_effects[ShaderManager::Effect::Brightness] = 100;
    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgb(255, 255, 255));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgba(128, 128, 128, 128));
}

TEST_F(EffectTransformTest, GhostEffect)
{
    // 25
    m_effects[ShaderManager::Effect::Ghost] = 25;
    auto mask = ShaderManager::Effect::Ghost;
    QRgb color = qRgba(0, 0, 0, 0);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), color);

    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgba(191, 0, 0, 191));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgba(75, 191, 150, 96));

    // 50
    m_effects[ShaderManager::Effect::Ghost] = 50;
    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgba(128, 0, 0, 128));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgba(50, 128, 100, 64));

    // 100
    m_effects[ShaderManager::Effect::Ghost] = 100;
    color = qRgba(255, 0, 0, 255);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgba(0, 0, 0, 0));

    color = qRgba(100, 255, 200, 128);
    ASSERT_EQ(EffectTransform::transformColor(mask, m_effects, color), qRgba(0, 0, 0, 0));
}

TEST_F(EffectTransformTest, FisheyeEffect)
{
    // 50
    m_effects[ShaderManager::Effect::Fisheye] = 50;
    auto mask = ShaderManager::Effect::Fisheye;
    QVector2D dst;
    EffectTransform::transformPoint(mask, m_effects, QSize(), QVector2D(0.51, 0.49), dst);
    ASSERT_EQ(std::round(dst.x() * 1000.0f) / 1000.0f, 0.502f);
    ASSERT_EQ(std::round(dst.y() * 1000.0f) / 1000.0f, 0.498f);

    // 200
    m_effects[ShaderManager::Effect::Fisheye] = 200;
    EffectTransform::transformPoint(mask, m_effects, QSize(), QVector2D(0.4, 0.68), dst);
    ASSERT_EQ(std::round(dst.x() * 1000.0f) / 1000.0f, 0.483f);
    ASSERT_EQ(std::round(dst.y() * 1000.0f) / 1000.0f, 0.531f);
}

TEST_F(EffectTransformTest, WhirlEffect)
{
    // 50
    m_effects[ShaderManager::Effect::Whirl] = 50;
    auto mask = ShaderManager::Effect::Whirl;
    QVector2D dst;
    EffectTransform::transformPoint(mask, m_effects, QSize(), QVector2D(0.51, 0.49), dst);
    ASSERT_EQ(std::round(dst.x() * 1000.0f) / 1000.0f, 0.499f);
    ASSERT_EQ(std::round(dst.y() * 1000.0f) / 1000.0f, 0.486f);

    // 200
    m_effects[ShaderManager::Effect::Whirl] = 200;
    EffectTransform::transformPoint(mask, m_effects, QSize(), QVector2D(0.4, 0.68), dst);
    ASSERT_EQ(std::round(dst.x() * 1000.0f) / 1000.0f, 0.633f);
    ASSERT_EQ(std::round(dst.y() * 1000.0f) / 1000.0f, 0.657f);
}

TEST_F(EffectTransformTest, PixelateEffect)
{
    // 5
    m_effects[ShaderManager::Effect::Pixelate] = 5;
    auto mask = ShaderManager::Effect::Pixelate;
    QVector2D dst;
    EffectTransform::transformPoint(mask, m_effects, QSize(), QVector2D(0.51, 0.05), dst);
    ASSERT_EQ(std::round(dst.x() * 1000.0f) / 1000.0f, 0.75f);
    ASSERT_EQ(std::round(dst.y() * 1000.0f) / 1000.0f, 0.25f);

    // 20
    m_effects[ShaderManager::Effect::Pixelate] = 20;
    EffectTransform::transformPoint(mask, m_effects, QSize(), QVector2D(0.97, 0.68), dst);
    ASSERT_EQ(std::round(dst.x() * 1000.0f) / 1000.0f, 1.0f);
    ASSERT_EQ(std::round(dst.y() * 1000.0f) / 1000.0f, 1.0f);
}

TEST_F(EffectTransformTest, MosaicEffect)
{
    // 50
    m_effects[ShaderManager::Effect::Mosaic] = 50;
    auto mask = ShaderManager::Effect::Mosaic;
    QVector2D dst;
    EffectTransform::transformPoint(mask, m_effects, QSize(), QVector2D(0.75, 0.25), dst);
    ASSERT_EQ(std::round(dst.x() * 1000.0f) / 1000.0f, 0.5f);
    ASSERT_EQ(std::round(dst.y() * 1000.0f) / 1000.0f, 0.5f);

    // 200
    m_effects[ShaderManager::Effect::Mosaic] = 200;
    EffectTransform::transformPoint(mask, m_effects, QSize(), QVector2D(0.8, 0.68), dst);
    ASSERT_EQ(std::round(dst.x() * 1000.0f) / 1000.0f, 0.8f);
    ASSERT_EQ(std::round(dst.y() * 1000.0f) / 1000.0f, 0.28f);
}
