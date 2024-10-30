#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QOpenGLShaderProgram>
#include <QFile>
#include <QOpenGLFunctions>
#include <scratchcpp/scratchconfiguration.h>
#include <shadermanager.h>
#include <graphicseffect.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

class ShaderManagerTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_context.create();
            ASSERT_TRUE(m_context.isValid());

            m_surface.setFormat(m_context.format());
            m_surface.create();
            Q_ASSERT(m_surface.isValid());
            m_context.makeCurrent(&m_surface);
        }

        void TearDown() override
        {
            ASSERT_EQ(m_context.surface(), &m_surface);
            m_context.doneCurrent();
        }

        QOpenGLContext m_context;
        QOffscreenSurface m_surface;
};

TEST_F(ShaderManagerTest, RegisteredEffects)
{
    // Color
    GraphicsEffect *effect = dynamic_cast<GraphicsEffect *>(ScratchConfiguration::getGraphicsEffect("color"));
    ASSERT_TRUE(effect);
    ASSERT_EQ(effect->effect(), ShaderManager::Effect::Color);

    // Brightness
    effect = dynamic_cast<GraphicsEffect *>(ScratchConfiguration::getGraphicsEffect("brightness"));
    ASSERT_TRUE(effect);
    ASSERT_EQ(effect->effect(), ShaderManager::Effect::Brightness);

    // Ghost
    effect = dynamic_cast<GraphicsEffect *>(ScratchConfiguration::getGraphicsEffect("ghost"));
    ASSERT_TRUE(effect);
    ASSERT_EQ(effect->effect(), ShaderManager::Effect::Ghost);
}

TEST_F(ShaderManagerTest, Constructor)
{
    ShaderManager manager1;
    ShaderManager manager2(&manager1);
    ASSERT_EQ(manager2.parent(), &manager1);
}

TEST_F(ShaderManagerTest, Instance)
{
    ASSERT_TRUE(ShaderManager::instance());
}

TEST_F(ShaderManagerTest, GetShaderProgram)
{
    ShaderManager manager;
    const std::unordered_map<ShaderManager::Effect, double> effects = { { ShaderManager::Effect::Color, 64.9 }, { ShaderManager::Effect::Ghost, 12.5 } };

    QOpenGLShaderProgram *program = manager.getShaderProgram(effects);
    ASSERT_EQ(program->parent(), &manager);
    ASSERT_TRUE(program->isLinked());

    auto shaders = program->shaders();
    ASSERT_EQ(shaders.size(), 2);
    QOpenGLShader *vert = shaders[0];
    QOpenGLShader *frag = shaders[1];
    ASSERT_EQ(vert->shaderType(), QOpenGLShader::Vertex);
    ASSERT_EQ(frag->shaderType(), QOpenGLShader::Fragment);

    // Test shader program cache
    program = manager.getShaderProgram(effects);
    ASSERT_EQ(program, program);

    program = manager.getShaderProgram(effects);
    ASSERT_EQ(program, program);
}

TEST_F(ShaderManagerTest, SetUniforms)
{
    QOpenGLFunctions glF(&m_context);
    glF.initializeOpenGLFunctions();
    ShaderManager manager;

    std::unordered_map<ShaderManager::Effect, double> effects = { { ShaderManager::Effect::Color, 64.9 }, { ShaderManager::Effect::Ghost, 12.5 } };
    QOpenGLShaderProgram *program = manager.getShaderProgram(effects);
    program->bind();
    manager.setUniforms(program, 4, QSize(), effects);

    GLint texUnit = -1;
    glF.glGetUniformiv(program->programId(), program->uniformLocation("u_skin"), &texUnit);
    ASSERT_EQ(texUnit, 4);

    GLfloat value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation("u_color"), &value);
    ASSERT_NE(value, 0.0f);

    value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation("u_ghost"), &value);
    ASSERT_NE(value, 0.0f);

    program->release();
}

TEST_F(ShaderManagerTest, ColorEffectValue)
{
    static const QString effectName = "color";
    static const QString uniformName = "u_" + effectName;
    static const ShaderManager::Effect effect = ShaderManager::Effect::Color;

    std::unordered_map<ShaderManager::Effect, float> values;

    QOpenGLFunctions glF(&m_context);
    glF.initializeOpenGLFunctions();
    ShaderManager manager;

    // In range
    std::unordered_map<ShaderManager::Effect, double> effects = { { effect, 64.9 } };
    QOpenGLShaderProgram *program = manager.getShaderProgram(effects);
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    GLfloat value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 0.3245f);
    ASSERT_EQ(values.at(effect), value);

    // Below the minimum
    effects[effect] = -395.7;
    program = manager.getShaderProgram(effects);
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(std::round(value * 100.0f) / 100.0f, 0.02f);
    ASSERT_EQ(values.at(effect), value);

    // Above the maximum
    effects[effect] = 579.05;
    program = manager.getShaderProgram(effects);
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(std::round(value * 100.0f) / 100.0f, 0.9f);
    ASSERT_EQ(values.at(effect), value);

    program->release();
}

TEST_F(ShaderManagerTest, BrightnessEffectValue)
{
    static const QString effectName = "brightness";
    static const QString uniformName = "u_" + effectName;
    static const ShaderManager::Effect effect = ShaderManager::Effect::Brightness;

    std::unordered_map<ShaderManager::Effect, float> values;

    QOpenGLFunctions glF(&m_context);
    glF.initializeOpenGLFunctions();
    ShaderManager manager;

    // In range
    std::unordered_map<ShaderManager::Effect, double> effects = { { effect, 4.6 } };
    QOpenGLShaderProgram *program = manager.getShaderProgram(effects);
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    GLfloat value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 0.046f);
    ASSERT_EQ(values.at(effect), value);

    // Below the minimum
    effects[effect] = -102.9;
    program = manager.getShaderProgram(effects);
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(value, -1.0f);
    ASSERT_EQ(values.at(effect), value);

    // Above the maximum
    effects[effect] = 353.2;
    program = manager.getShaderProgram(effects);
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 1.0f);
    ASSERT_EQ(values.at(effect), value);

    program->release();
}

TEST_F(ShaderManagerTest, GhostEffectValue)
{
    static const QString effectName = "ghost";
    static const QString uniformName = "u_" + effectName;
    static const ShaderManager::Effect effect = ShaderManager::Effect::Ghost;

    std::unordered_map<ShaderManager::Effect, float> values;

    QOpenGLFunctions glF(&m_context);
    glF.initializeOpenGLFunctions();
    ShaderManager manager;

    // In range
    std::unordered_map<ShaderManager::Effect, double> effects = { { effect, 58.5 } };
    QOpenGLShaderProgram *program = manager.getShaderProgram(effects);
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    GLfloat value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(std::round(value * 1000.0f) / 1000.0f, 0.415f);
    ASSERT_EQ(values.at(effect), value);

    // Below the minimum
    effects[effect] = -20.8;
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 1.0f);
    ASSERT_EQ(values.at(effect), value);

    // Above the maximum
    effects[effect] = 248.2;
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 0.0f);
    ASSERT_EQ(values.at(effect), value);

    program->release();
}

TEST_F(ShaderManagerTest, FisheyeEffectValue)
{
    static const QString effectName = "fisheye";
    static const QString uniformName = "u_" + effectName;
    static const ShaderManager::Effect effect = ShaderManager::Effect::Fisheye;

    std::unordered_map<ShaderManager::Effect, float> values;

    QOpenGLFunctions glF(&m_context);
    glF.initializeOpenGLFunctions();
    ShaderManager manager;

    // In range
    std::unordered_map<ShaderManager::Effect, double> effects = { { effect, 58.5 } };
    QOpenGLShaderProgram *program = manager.getShaderProgram(effects);
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    GLfloat value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 1.585f);
    ASSERT_EQ(values.at(effect), value);

    effects[effect] = -20.8;
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 0.792f);
    ASSERT_EQ(values.at(effect), value);

    // Below the minimum
    effects[effect] = -101;
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 0.0f);
    ASSERT_EQ(values.at(effect), value);

    program->release();
}

TEST_F(ShaderManagerTest, WhirlEffectValue)
{
    static const QString effectName = "whirl";
    static const QString uniformName = "u_" + effectName;
    static const ShaderManager::Effect effect = ShaderManager::Effect::Whirl;

    std::unordered_map<ShaderManager::Effect, float> values;

    QOpenGLFunctions glF(&m_context);
    glF.initializeOpenGLFunctions();
    ShaderManager manager;

    // In range
    std::unordered_map<ShaderManager::Effect, double> effects = { { effect, 58.5 } };
    QOpenGLShaderProgram *program = manager.getShaderProgram(effects);
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    GLfloat value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(std::round(value * 1000) / 1000, 1.021f);
    ASSERT_EQ(values.at(effect), value);

    effects[effect] = -20.8;
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(std::round(value * 1000) / 1000, -0.363f);
    ASSERT_EQ(values.at(effect), value);

    program->release();
}

TEST_F(ShaderManagerTest, PixelateEffectValue)
{
    static const QString effectName = "pixelate";
    static const QString uniformName = "u_" + effectName;
    static const ShaderManager::Effect effect = ShaderManager::Effect::Pixelate;

    std::unordered_map<ShaderManager::Effect, float> values;

    QOpenGLFunctions glF(&m_context);
    glF.initializeOpenGLFunctions();
    ShaderManager manager;

    // In range
    std::unordered_map<ShaderManager::Effect, double> effects = { { effect, 58.5 } };
    QOpenGLShaderProgram *program = manager.getShaderProgram(effects);
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    GLfloat value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 5.85f);
    ASSERT_EQ(values.at(effect), value);

    effects[effect] = -20.8;
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 2.08f);
    ASSERT_EQ(values.at(effect), value);

    program->release();
}

TEST_F(ShaderManagerTest, MosaicEffectValue)
{
    static const QString effectName = "mosaic";
    static const QString uniformName = "u_" + effectName;
    static const ShaderManager::Effect effect = ShaderManager::Effect::Mosaic;

    std::unordered_map<ShaderManager::Effect, float> values;

    QOpenGLFunctions glF(&m_context);
    glF.initializeOpenGLFunctions();
    ShaderManager manager;

    // In range
    std::unordered_map<ShaderManager::Effect, double> effects = { { effect, 58.5 } };
    QOpenGLShaderProgram *program = manager.getShaderProgram(effects);
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    GLfloat value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 7.0f);
    ASSERT_EQ(values.at(effect), value);

    effects[effect] = -21.8;
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 3.0f);
    ASSERT_EQ(values.at(effect), value);

    // Below the minimum
    effects[effect] = 4;
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 1.0f);
    ASSERT_EQ(values.at(effect), value);

    // Above the maximum
    effects[effect] = 5120;
    program->bind();
    manager.setUniforms(program, 0, QSize(), effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program->programId(), program->uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 512.0f);
    ASSERT_EQ(values.at(effect), value);

    program->release();
}

TEST_F(ShaderManagerTest, Effects)
{
    ASSERT_EQ(
        ShaderManager::effects(),
        std::unordered_set<ShaderManager::Effect>(
            { ShaderManager::Effect::Color,
              ShaderManager::Effect::Brightness,
              ShaderManager::Effect::Ghost,
              ShaderManager::Effect::Fisheye,
              ShaderManager::Effect::Whirl,
              ShaderManager::Effect::Pixelate,
              ShaderManager::Effect::Mosaic }));
}

TEST_F(ShaderManagerTest, EffectShapeChanges)
{
    ASSERT_FALSE(ShaderManager::effectShapeChanges(ShaderManager::Effect::Color));
    ASSERT_FALSE(ShaderManager::effectShapeChanges(ShaderManager::Effect::Brightness));
    ASSERT_FALSE(ShaderManager::effectShapeChanges(ShaderManager::Effect::Ghost));
    ASSERT_TRUE(ShaderManager::effectShapeChanges(ShaderManager::Effect::Fisheye));
    ASSERT_TRUE(ShaderManager::effectShapeChanges(ShaderManager::Effect::Whirl));
    ASSERT_TRUE(ShaderManager::effectShapeChanges(ShaderManager::Effect::Pixelate));
    ASSERT_TRUE(ShaderManager::effectShapeChanges(ShaderManager::Effect::Mosaic));
}
