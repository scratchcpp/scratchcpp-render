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

static const QString VERTEX_SHADER_SRC = ":/qt/qml/ScratchCPP/Render/shaders/sprite.vert";
static const QString FRAGMENT_SHADER_SRC = ":/qt/qml/ScratchCPP/Render/shaders/sprite.frag";

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

            QFile vertSource(VERTEX_SHADER_SRC);
            vertSource.open(QFile::ReadOnly);
            m_vertexShader = vertSource.readAll();
            ASSERT_FALSE(m_vertexShader.isEmpty());

            QFile fragSource(FRAGMENT_SHADER_SRC);
            fragSource.open(QFile::ReadOnly);
            m_fragmentShader = fragSource.readAll();
            ASSERT_FALSE(m_fragmentShader.isEmpty());
        }

        void TearDown() override
        {
            ASSERT_EQ(m_context.surface(), &m_surface);
            m_context.doneCurrent();
        }

        QOpenGLContext m_context;
        QOffscreenSurface m_surface;
        QByteArray m_vertexShader;
        QByteArray m_fragmentShader;
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
    static const QByteArray vertHeader = "#version 330 core\n#define lowp\n#define mediump\n#define highp\n#line 1\n";
    static const QByteArray fragHeader =
        "#version 330\n#ifdef GL_KHR_blend_equation_advanced\n#extension GL_ARB_fragment_coord_conventions : enable\n#extension GL_KHR_blend_equation_advanced : enable\n#endif\n#define lowp\n#define "
        "mediump\n#define highp\n#line 1\n";

    // Color and ghost
    ShaderManager manager;
    const std::unordered_map<ShaderManager::Effect, double> effects1 = { { ShaderManager::Effect::Color, 64.9 }, { ShaderManager::Effect::Ghost, 12.5 } };

    QOpenGLShaderProgram *program1 = manager.getShaderProgram(effects1);
    ASSERT_EQ(program1->parent(), &manager);
    ASSERT_TRUE(program1->isLinked());

    auto shaders = program1->shaders();
    ASSERT_EQ(shaders.size(), 2);
    QOpenGLShader *vert = shaders[0];
    QOpenGLShader *frag = shaders[1];
    ASSERT_EQ(vert->shaderType(), QOpenGLShader::Vertex);
    ASSERT_EQ(vert->sourceCode(), vertHeader + m_vertexShader);
    ASSERT_EQ(frag->shaderType(), QOpenGLShader::Fragment);
    ASSERT_EQ(frag->sourceCode(), fragHeader + "#define ENABLE_ghost\n#define ENABLE_color\n" + m_fragmentShader);

    // Brightness and ghost
    const std::unordered_map<ShaderManager::Effect, double> effects2 = { { ShaderManager::Effect::Brightness, 64.9 }, { ShaderManager::Effect::Ghost, 12.5 } };

    QOpenGLShaderProgram *program2 = manager.getShaderProgram(effects2);
    ASSERT_EQ(program2->parent(), &manager);
    ASSERT_TRUE(program2->isLinked());

    shaders = program2->shaders();
    ASSERT_EQ(shaders.size(), 2);
    vert = shaders[0];
    frag = shaders[1];
    ASSERT_EQ(vert->shaderType(), QOpenGLShader::Vertex);
    ASSERT_EQ(vert->sourceCode(), vertHeader + m_vertexShader);
    ASSERT_EQ(frag->shaderType(), QOpenGLShader::Fragment);
    ASSERT_EQ(frag->sourceCode(), fragHeader + "#define ENABLE_ghost\n#define ENABLE_brightness\n" + m_fragmentShader);

    // Test shader program cache
    QOpenGLShaderProgram *program = manager.getShaderProgram(effects1);
    ASSERT_EQ(program, program1);

    program = manager.getShaderProgram(effects2);
    ASSERT_EQ(program, program2);

    // Color and brightness where color effect value is zero
    const std::unordered_map<ShaderManager::Effect, double> effects3 = { { ShaderManager::Effect::Color, 0.0 }, { ShaderManager::Effect::Brightness, 22.3 } };

    program = manager.getShaderProgram(effects3);
    ASSERT_EQ(program->parent(), &manager);
    ASSERT_TRUE(program->isLinked());

    shaders = program->shaders();
    ASSERT_EQ(shaders.size(), 2);
    vert = shaders[0];
    frag = shaders[1];
    ASSERT_EQ(vert->shaderType(), QOpenGLShader::Vertex);
    ASSERT_EQ(vert->sourceCode(), vertHeader + m_vertexShader);
    ASSERT_EQ(frag->shaderType(), QOpenGLShader::Fragment);
    ASSERT_EQ(frag->sourceCode(), fragHeader + "#define ENABLE_brightness\n" + m_fragmentShader);
}

TEST_F(ShaderManagerTest, SetUniforms)
{
    QOpenGLFunctions glF(&m_context);
    glF.initializeOpenGLFunctions();
    ShaderManager manager;
    QOpenGLShaderProgram program;
    program.addShaderFromSourceCode(QOpenGLShader::Vertex, "#version 330 core\n" + m_vertexShader);
    program.addShaderFromSourceCode(QOpenGLShader::Fragment, "#version 330\n#define ENABLE_color\n#define ENABLE_ghost\n" + m_fragmentShader);

    std::unordered_map<ShaderManager::Effect, double> effects = { { ShaderManager::Effect::Color, 64.9 }, { ShaderManager::Effect::Ghost, 12.5 } };
    program.bind();
    manager.setUniforms(&program, 4, effects);

    GLint texUnit = -1;
    glF.glGetUniformiv(program.programId(), program.uniformLocation("u_skin"), &texUnit);
    ASSERT_EQ(texUnit, 4);

    GLfloat value = 0.0f;
    glF.glGetUniformfv(program.programId(), program.uniformLocation("u_color"), &value);
    ASSERT_NE(value, 0.0f);

    value = 0.0f;
    glF.glGetUniformfv(program.programId(), program.uniformLocation("u_ghost"), &value);
    ASSERT_NE(value, 0.0f);

    program.release();
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
    QOpenGLShaderProgram program;
    program.addShaderFromSourceCode(QOpenGLShader::Vertex, "#version 330 core\n" + m_vertexShader);
    program.addShaderFromSourceCode(QOpenGLShader::Fragment, "#version 330\n#define ENABLE_" + effectName + "\n" + m_fragmentShader);

    // In range
    std::unordered_map<ShaderManager::Effect, double> effects = { { effect, 64.9 } };
    program.bind();
    manager.setUniforms(&program, 0, effects);
    manager.getUniformValuesForEffects(effects, values);

    GLfloat value = 0.0f;
    glF.glGetUniformfv(program.programId(), program.uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 0.3245f);
    ASSERT_EQ(values.at(effect), value);

    // Below the minimum
    effects[effect] = -395.7;
    program.bind();
    manager.setUniforms(&program, 0, effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program.programId(), program.uniformLocation(uniformName), &value);
    ASSERT_EQ(std::round(value * 100.0f) / 100.0f, 0.02f);
    ASSERT_EQ(values.at(effect), value);

    // Above the maximum
    effects[effect] = 579.05;
    program.bind();
    manager.setUniforms(&program, 0, effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program.programId(), program.uniformLocation(uniformName), &value);
    ASSERT_EQ(std::round(value * 100.0f) / 100.0f, 0.9f);
    ASSERT_EQ(values.at(effect), value);

    program.release();
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
    QOpenGLShaderProgram program;
    program.addShaderFromSourceCode(QOpenGLShader::Vertex, "#version 330 core\n" + m_vertexShader);
    program.addShaderFromSourceCode(QOpenGLShader::Fragment, "#version 330\n#define ENABLE_" + effectName + "\n" + m_fragmentShader);

    // In range
    std::unordered_map<ShaderManager::Effect, double> effects = { { effect, 4.6 } };
    program.bind();
    manager.setUniforms(&program, 0, effects);
    manager.getUniformValuesForEffects(effects, values);

    GLfloat value = 0.0f;
    glF.glGetUniformfv(program.programId(), program.uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 0.046f);
    ASSERT_EQ(values.at(effect), value);

    // Below the minimum
    effects[effect] = -102.9;
    program.bind();
    manager.setUniforms(&program, 0, effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program.programId(), program.uniformLocation(uniformName), &value);
    ASSERT_EQ(value, -1.0f);
    ASSERT_EQ(values.at(effect), value);

    // Above the maximum
    effects[effect] = 353.2;
    program.bind();
    manager.setUniforms(&program, 0, effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program.programId(), program.uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 1.0f);
    ASSERT_EQ(values.at(effect), value);

    program.release();
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
    QOpenGLShaderProgram program;
    program.addShaderFromSourceCode(QOpenGLShader::Vertex, "#version 330 core\n" + m_vertexShader);
    program.addShaderFromSourceCode(QOpenGLShader::Fragment, "#version 330\n#define ENABLE_" + effectName + "\n" + m_fragmentShader);

    // In range
    std::unordered_map<ShaderManager::Effect, double> effects = { { effect, 58.5 } };
    program.bind();
    manager.setUniforms(&program, 0, effects);
    manager.getUniformValuesForEffects(effects, values);

    GLfloat value = 0.0f;
    glF.glGetUniformfv(program.programId(), program.uniformLocation(uniformName), &value);
    ASSERT_EQ(std::round(value * 1000.0f) / 1000.0f, 0.415f);
    ASSERT_EQ(values.at(effect), value);

    // Below the minimum
    effects[effect] = -20.8;
    program.bind();
    manager.setUniforms(&program, 0, effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program.programId(), program.uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 1.0f);
    ASSERT_EQ(values.at(effect), value);

    // Above the maximum
    effects[effect] = 248.2;
    program.bind();
    manager.setUniforms(&program, 0, effects);
    manager.getUniformValuesForEffects(effects, values);

    value = 0.0f;
    glF.glGetUniformfv(program.programId(), program.uniformLocation(uniformName), &value);
    ASSERT_EQ(value, 0.0f);
    ASSERT_EQ(values.at(effect), value);

    program.release();
}
