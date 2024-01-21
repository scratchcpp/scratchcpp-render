// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/costume.h>

#include "svgskin.h"

using namespace scratchcpprender;

static const int MAX_TEXTURE_DIMENSION = 2048;
static const int INDEX_OFFSET = 8;

SVGSkin::SVGSkin(libscratchcpp::Costume *costume, bool antialiasing) :
    Skin(),
    m_antialiasing(antialiasing)
{
    if (!costume)
        return;

    // Load SVG data
    m_svgRen.load(QByteArray(static_cast<const char *>(costume->data()), costume->dataSize()));

    // Calculate maximum index (larger images will only be scaled up)
    const QRectF viewBox = m_svgRen.viewBox();

    if (viewBox.width() == 0 || viewBox.height() == 0)
        return;

    const int i1 = std::log2(MAX_TEXTURE_DIMENSION / viewBox.width()) + INDEX_OFFSET;
    const int i2 = std::log2(MAX_TEXTURE_DIMENSION / viewBox.height()) + INDEX_OFFSET;
    m_maxIndex = std::min(i1, i2);

    // Create all possible textures (the 1.0 scale is stored at INDEX_OFFSET)
    // TODO: Is this necessary?
    for (int i = 0; i <= m_maxIndex; i++)
        createScaledTexture(i);
}

SVGSkin::~SVGSkin()
{
    for (const auto &[index, texture] : m_textures)
        m_textureObjects[texture].release();
}

Texture SVGSkin::getTexture(double scale) const
{
    // https://github.com/scratchfoundation/scratch-render/blob/423bb700c36b8c1c0baae1e2413878a4f778849a/src/SVGSkin.js#L158-L176
    int mipLevel = std::max(std::ceil(std::log2(scale)) + INDEX_OFFSET, 0.0);

    // Limit to maximum index
    mipLevel = std::min(mipLevel, m_maxIndex);

    auto it = m_textures.find(mipLevel);

    if (it == m_textures.cend())
        return const_cast<SVGSkin *>(this)->createScaledTexture(mipLevel); // TODO: Remove that awful const_cast ;)
    else
        return m_textureObjects.at(it->second);
}

double SVGSkin::getTextureScale(const Texture &texture) const
{
    auto it = m_textureIndexes.find(texture.handle());

    if (it != m_textureIndexes.cend())
        return std::pow(2, it->second - INDEX_OFFSET);

    return 1;
}

void SVGSkin::paint(QPainter *painter)
{
    const QPaintDevice *device = painter->device();
    m_svgRen.render(painter, QRectF(0, 0, device->width(), device->height()));
}

Texture SVGSkin::createScaledTexture(int index)
{
    Q_ASSERT(m_textures.find(index) == m_textures.cend());
    auto it = m_textures.find(index);

    if (it != m_textures.cend())
        return m_textureObjects[it->second];

    const double scale = std::pow(2, index - INDEX_OFFSET);
    const QRect viewBox = m_svgRen.viewBox();
    const double width = viewBox.width() * scale;
    const double height = viewBox.height() * scale;

    if (width > MAX_TEXTURE_DIMENSION || height > MAX_TEXTURE_DIMENSION) {
        Q_ASSERT(false); // this shouldn't happen because indexes are limited to the max index
        return Texture();
    }

    const Texture texture = createAndPaintTexture(viewBox.width() * scale, viewBox.height() * scale, m_antialiasing);

    if (texture.isValid()) {
        m_textures[index] = texture.handle();
        m_textureIndexes[texture.handle()] = index;
        m_textureObjects[texture.handle()] = texture;
    }

    return texture;
}
