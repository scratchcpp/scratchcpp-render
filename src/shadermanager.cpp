// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <QFile>
#include <scratchcpp/scratchconfiguration.h>

#include "shadermanager.h"
#include "graphicseffect.h"

using namespace scratchcpprender;

using ConverterFunc = float (*)(float);

static const double pi = std::acos(-1); // TODO: Use std::numbers::pi in C++20

static float wrapClamp(float n, float min, float max)
{
    // TODO: Move this to a separate class
    const float range = max - min;
    return n - (std::floor((n - min) / range) * range);
}

static const QString VERTEX_SHADER_SRC = ":/qt/qml/ScratchCPP/Render/shaders/sprite.vert";
static const QString FRAGMENT_SHADER_SRC = ":/qt/qml/ScratchCPP/Render/shaders/sprite.frag";

#if defined(Q_OS_WASM)
static const QString SHADER_PREFIX = ""; // compiles, but doesn't work?
#elif defined(Q_OS_ANDROID)
static const QString SHADER_PREFIX = "#version 300 es\n";
#else
static const QString SHADER_PREFIX = "#version 140\n";
#endif

static const char *TEXTURE_UNIT_UNIFORM = "u_skin";

static const std::unordered_map<ShaderManager::Effect, const char *> EFFECT_TO_NAME = {
    { ShaderManager::Effect::Color, "color" },
    { ShaderManager::Effect::Brightness, "brightness" },
    { ShaderManager::Effect::Ghost, "ghost" },
    { ShaderManager::Effect::Fisheye, "fisheye" },
    { ShaderManager::Effect::Whirl, "whirl" }
};

static const std::unordered_map<ShaderManager::Effect, const char *> EFFECT_UNIFORM_NAME = {
    { ShaderManager::Effect::Color, "u_color" },
    { ShaderManager::Effect::Brightness, "u_brightness" },
    { ShaderManager::Effect::Ghost, "u_ghost" },
    { ShaderManager::Effect::Fisheye, "u_fisheye" },
    { ShaderManager::Effect::Whirl, "u_whirl" }
};

static const std::unordered_map<ShaderManager::Effect, ConverterFunc> EFFECT_CONVERTER = {
    { ShaderManager::Effect::Color, [](float x) { return wrapClamp(x / 200.0f, 0.0f, 1.0f); } },
    { ShaderManager::Effect::Brightness, [](float x) { return std::clamp(x, -100.0f, 100.0f) / 100.0f; } },
    { ShaderManager::Effect::Ghost, [](float x) { return 1 - std::clamp(x, 0.0f, 100.0f) / 100.0f; } },
    { ShaderManager::Effect::Fisheye, [](float x) { return std::max(0.0f, (x + 100.0f) / 100.0f); } },
    { ShaderManager::Effect::Whirl, [](float x) { return x * (float)pi / 180.0f; } }
};

static const std::unordered_map<ShaderManager::Effect, bool> EFFECT_SHAPE_CHANGES = {
    { ShaderManager::Effect::Color, false },
    { ShaderManager::Effect::Brightness, false },
    { ShaderManager::Effect::Ghost, false },
    { ShaderManager::Effect::Fisheye, true },
    { ShaderManager::Effect::Whirl, true }
};

Q_GLOBAL_STATIC(ShaderManager, globalInstance)

ShaderManager::Registrar ShaderManager::m_registrar;

ShaderManager::ShaderManager(QObject *parent) :
    QObject(parent)
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    Q_ASSERT(context);

    if (!context) {
        qWarning("ShaderManager must be constructed with a valid OpenGL context.");
        return;
    }

    // Compile the vertex shader (it will be used in any shader program)
    QByteArray vertexShaderSource;
    QFile vertSource(VERTEX_SHADER_SRC);
    vertSource.open(QFile::ReadOnly);
    vertexShaderSource = SHADER_PREFIX.toUtf8() + vertSource.readAll();

    m_vertexShader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    m_vertexShader->compileSourceCode(vertexShaderSource);
    Q_ASSERT(m_vertexShader->isCompiled());

    // Load the fragment shader source code
    QFile fragSource(FRAGMENT_SHADER_SRC);
    fragSource.open(QFile::ReadOnly);
    m_fragmentShaderSource = fragSource.readAll();
    Q_ASSERT(!m_fragmentShaderSource.isEmpty());
}

ShaderManager *ShaderManager::instance()
{
    return globalInstance;
}

QOpenGLShaderProgram *ShaderManager::getShaderProgram(const std::unordered_map<Effect, double> &effectValues)
{
    int effectBits = 0;
    bool firstSet = false;

    for (const auto &[effect, value] : effectValues) {
        if (value != 0)
            effectBits |= static_cast<int>(effect);
    }

    // Find the selected effect combination
    auto it = m_shaderPrograms.find(effectBits);

    if (it == m_shaderPrograms.cend()) {
        // Create a new shader program if this combination doesn't exist yet
        QOpenGLShaderProgram *program = createShaderProgram(effectValues);

        if (program)
            m_shaderPrograms[effectBits] = program;

        return program;
    } else
        return it->second;
}

void ShaderManager::getUniformValuesForEffects(const std::unordered_map<Effect, double> &effectValues, std::unordered_map<Effect, float> &dst)
{
    dst.clear();

    for (const auto &[effect, name] : EFFECT_TO_NAME) {
        const auto it = effectValues.find(effect);
        double value;

        if (it == effectValues.cend())
            value = 0; // reset the effect
        else
            value = it->second;

        auto converter = EFFECT_CONVERTER.at(effect);
        dst[effect] = converter(value);
    }
}

void ShaderManager::setUniforms(QOpenGLShaderProgram *program, int textureUnit, const std::unordered_map<Effect, double> &effectValues)
{
    // Set the texture unit
    program->setUniformValue(TEXTURE_UNIT_UNIFORM, textureUnit);

    // Set uniform values
    std::unordered_map<Effect, float> values;
    getUniformValuesForEffects(effectValues, values);

    for (const auto &[effect, value] : values)
        program->setUniformValue(EFFECT_UNIFORM_NAME.at(effect), value);
}

void ShaderManager::registerEffects()
{
    // Register graphic effects in libscratchcpp
    for (const auto &[effect, name] : EFFECT_TO_NAME) {
        auto effectObj = std::make_shared<GraphicsEffect>(effect, name);
        libscratchcpp::ScratchConfiguration::registerGraphicsEffect(effectObj);
    }
}

QOpenGLShaderProgram *ShaderManager::createShaderProgram(const std::unordered_map<Effect, double> &effectValues)
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    Q_ASSERT(context && m_vertexShader);

    if (!context || !m_vertexShader)
        return nullptr;

    // Version must be defined in the first line
    QByteArray fragSource = SHADER_PREFIX.toUtf8();

    // Add defines for the effects
    for (const auto &[effect, value] : effectValues) {
        if (value != 0) {
            fragSource.push_back("#define ENABLE_");
            fragSource.push_back(EFFECT_TO_NAME.at(effect));
            fragSource.push_back('\n');
        }
    }

    // Add the actual fragment shader
    fragSource.push_back(m_fragmentShaderSource);

    QOpenGLShaderProgram *program = new QOpenGLShaderProgram(this);
    program->addShader(m_vertexShader);
    program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragSource);
    program->link();

    return program;
}
