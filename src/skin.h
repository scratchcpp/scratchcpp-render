// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QPointF>
#include <QSizeF>
#include <QtOpenGL>

namespace scratchcpprender
{

class Texture;

class Skin
{
    public:
        Skin();
        Skin(const Skin &) = delete;
        virtual ~Skin() { }

        virtual Texture getTexture(double scale) const = 0;
        virtual double getTextureScale(const Texture &texture) const = 0;

    protected:
        Texture createAndPaintTexture(int width, int height);
        virtual void paint(QPainter *painter) = 0;

    private:
        static inline std::vector<std::shared_ptr<QOpenGLTexture>> m_textures;
        static inline bool m_destroyConnected = false;
};

} // namespace scratchcpprender
