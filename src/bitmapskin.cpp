// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/costume.h>

#include "bitmapskin.h"

using namespace scratchcpprender;

BitmapSkin::BitmapSkin(libscratchcpp::Costume *costume) :
    Skin()
{
    if (!costume)
        return;

    // Read image data
    QBuffer buffer;
    buffer.open(QBuffer::WriteOnly);
    buffer.write(static_cast<const char *>(costume->data()), costume->dataSize());
    buffer.close();
    const char *format;

    {
        QImageReader reader(&buffer);
        format = reader.format();
    }

    buffer.close();
    m_image.load(&buffer, format);

    // Paint the image into a texture
    m_texture = createAndPaintTexture(m_image.width(), m_image.height(), false);
    m_textureSize.setWidth(m_image.width());
    m_textureSize.setHeight(m_image.height());
    Q_ASSERT(m_texture.isValid());
}

BitmapSkin::~BitmapSkin()
{
    m_texture.release();
}

Texture BitmapSkin::getTexture(double scale) const
{
    return m_texture;
}

double BitmapSkin::getTextureScale(const Texture &texture) const
{
    return 1;
}

void BitmapSkin::paint(QPainter *painter)
{
    painter->drawImage(m_image.rect(), m_image, m_image.rect());
}
