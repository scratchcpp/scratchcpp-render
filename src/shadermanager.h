// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QObject>
#include <memory>

class QOpenGLShaderProgram;
class QOpenGLShader;

namespace scratchcpprender
{

class ShaderManager : public QObject
{
    public:
        enum class Effect
        {
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

    private:
        struct Registrar
        {
                Registrar() { registerEffects(); }
        };

        static void registerEffects();

        QOpenGLShaderProgram *createShaderProgram(const std::unordered_map<Effect, double> &effectValues);

        static Registrar m_registrar;

        QOpenGLShader *m_vertexShader = nullptr;
        std::unordered_map<int, QOpenGLShaderProgram *> m_shaderPrograms;
        QByteArray m_fragmentShaderSource;
};

} // namespace scratchcpprender
