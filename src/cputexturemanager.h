// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QPoint>
#include <QtOpenGL>
#include <unordered_map>

namespace scratchcpprender
{

class Texture;

class CpuTextureManager
{
    public:
        CpuTextureManager();
        ~CpuTextureManager();

        GLubyte *getTextureData(const Texture &texture);
        const std::vector<QPoint> &getTextureConvexHullPoints(const Texture &texture);

    private:
        bool addTexture(const Texture &texture);

        std::unordered_map<GLuint, GLubyte *> m_textureData;
        std::unordered_map<GLuint, std::vector<QPoint>> m_convexHullPoints;
};

} // namespace scratchcpprender
