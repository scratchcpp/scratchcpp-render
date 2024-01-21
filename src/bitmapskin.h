// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "skin.h"
#include "texture.h"

namespace libscratchcpp
{

class Costume;

}

namespace scratchcpprender
{

class BitmapSkin : public Skin
{
    public:
        BitmapSkin(libscratchcpp::Costume *costume);
        ~BitmapSkin();

        Texture getTexture(double scale) const override;
        double getTextureScale(const Texture &texture) const override;

    protected:
        void paint(QPainter *painter) override;

    private:
        Texture m_texture;
        QSize m_textureSize;
        QImage m_image;
};

} // namespace scratchcpprender
