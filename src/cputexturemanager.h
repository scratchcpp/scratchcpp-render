// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QPoint>
#include <QtOpenGL>
#include <unordered_map>

#include "shadermanager.h"

namespace scratchcpprender
{

class Texture;

class CpuTextureManager
{
    public:
        CpuTextureManager();
        ~CpuTextureManager();

        GLubyte *getTextureData(const Texture &texture);
        void getTextureConvexHullPoints(
            const Texture &texture,
            const QSize &skinSize,
            ShaderManager::Effect effectMask,
            const std::unordered_map<ShaderManager::Effect, double> &effects,
            std::vector<QPoint> &dst);

        QRgb getPointColor(const Texture &texture, int x, int y, ShaderManager::Effect effectMask, const std::unordered_map<ShaderManager::Effect, double> &effects);
        bool textureContainsPoint(const Texture &texture, const QPointF &localPoint, ShaderManager::Effect effectMask, const std::unordered_map<ShaderManager::Effect, double> &effects);

        void removeTexture(const Texture &texture);

    private:
        bool addTexture(const Texture &tex);
        bool readTexture(
            const Texture &texture,
            const QSize &skinSize,
            ShaderManager::Effect effectMask,
            const std::unordered_map<ShaderManager::Effect, double> &effects,
            GLubyte **data,
            std::vector<QPoint> &points) const;

        static inline GLuint m_fbo = 0;          // single FBO for all texture managers
        std::unordered_map<GLuint, GLubyte *> m_textureData;
        std::unordered_map<GLuint, std::vector<QPoint>> m_convexHullPoints;
};

} // namespace scratchcpprender
