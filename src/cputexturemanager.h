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

        bool textureContainsPoint(const Texture &texture, const QPointF &localPoint);

        void removeTexture(const Texture &texture);

    private:
        bool addTexture(const Texture &texture);
        int getPointAlpha(const Texture &texture, int x, int y);

        std::unordered_map<GLuint, GLubyte *> m_textureData;
        std::unordered_map<GLuint, std::vector<QPoint>> m_convexHullPoints;
};

} // namespace scratchcpprender
