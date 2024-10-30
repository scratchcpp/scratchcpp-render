// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QObject>
#include <memory>
#include <unordered_set>

class QOpenGLShaderProgram;
class QOpenGLShader;

namespace scratchcpprender
{

class ShaderManager : public QObject
{
    public:
        enum class Effect
        {
            NoEffect = 0,
            Color = 1 << 0,
            Brightness = 1 << 1,
            Ghost = 1 << 2,
            Fisheye = 1 << 3,
            Whirl = 1 << 4,
            Pixelate = 1 << 5,
            Mosaic = 1 << 6
        };

        explicit ShaderManager(QObject *parent = nullptr);

        static ShaderManager *instance();

        QOpenGLShaderProgram *getShaderProgram(const std::unordered_map<Effect, double> &effectValues);
        static void getUniformValuesForEffects(const std::unordered_map<Effect, double> &effectValues, std::unordered_map<Effect, float> &dst);
        void setUniforms(QOpenGLShaderProgram *program, int textureUnit, const QSize skinSize, const std::unordered_map<Effect, double> &effectValues);

        static const std::unordered_set<Effect> &effects();
        static bool effectShapeChanges(Effect effect);

    private:
        struct Registrar
        {
                Registrar() { registerEffects(); }
        };

        static void registerEffects();

        QOpenGLShaderProgram *createShaderProgram(const std::unordered_map<Effect, double> &effectValues);

        static Registrar m_registrar;
        static std::unordered_set<Effect> m_effects;

        QOpenGLShader *m_vertexShader = nullptr;
        std::unordered_map<int, QOpenGLShaderProgram *> m_shaderPrograms;
        QByteArray m_fragmentShaderSource;
};

inline ShaderManager::Effect operator|(ShaderManager::Effect a, ShaderManager::Effect b)
{
    return static_cast<ShaderManager::Effect>(static_cast<int>(a) | static_cast<int>(b));
}

inline ShaderManager::Effect operator|=(ShaderManager::Effect &a, ShaderManager::Effect b)
{
    return static_cast<ShaderManager::Effect>((int &)a |= static_cast<int>(b));
}

inline ShaderManager::Effect operator&(ShaderManager::Effect a, ShaderManager::Effect b)
{
    return static_cast<ShaderManager::Effect>(static_cast<int>(a) & static_cast<int>(b));
}

inline ShaderManager::Effect operator&=(ShaderManager::Effect &a, ShaderManager::Effect b)
{
    return static_cast<ShaderManager::Effect>((int &)a &= static_cast<int>(b));
}

inline ShaderManager::Effect operator~(ShaderManager::Effect a)
{
    return static_cast<ShaderManager::Effect>(~static_cast<int>(a));
}

inline bool operator==(ShaderManager::Effect a, int b)
{
    return static_cast<int>(a) == b;
}

inline bool operator==(int a, ShaderManager::Effect b)
{
    return a == static_cast<int>(b);
}

inline bool operator!=(ShaderManager::Effect a, int b)
{
    return static_cast<int>(a) != b;
}

inline bool operator!=(int a, ShaderManager::Effect b)
{
    return a != static_cast<int>(b);
}

} // namespace scratchcpprender
