// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QSvgRenderer>

#include "skin.h"
#include "texture.h"

namespace scratchcpprender
{

class SVGSkin : public Skin
{
    public:
        SVGSkin(libscratchcpp::Costume *costume, bool antialiasing = true);
        ~SVGSkin();

        Texture getTexture(double scale) const override;
        double getTextureScale(const Texture &texture) const override;

    protected:
        void paint(QPainter *painter) override;

    private:
        Texture createScaledTexture(int index);

        std::unordered_map<int, GLuint> m_textures;
        std::unordered_map<GLuint, int> m_textureIndexes; // reverse map of m_textures
        std::unordered_map<GLuint, Texture> m_textureObjects;
        QSvgRenderer m_svgRen;
        int m_maxIndex = 0;
        bool m_antialiasing = false;
};

} // namespace scratchcpprender
