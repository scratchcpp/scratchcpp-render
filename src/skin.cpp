// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/costume.h>

#include "skin.h"
#include "texture.h"

using namespace scratchcpprender;

Skin::Skin()
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    Q_ASSERT(context);

    if (!m_connectedCtx || (context && context != m_connectedCtx)) {
        QObject::connect(context, &QOpenGLContext::aboutToBeDestroyed, []() {
            // Destroy textures
            m_textures.clear();
        });

        m_connectedCtx = context;
    }
}

Texture Skin::createAndPaintTexture(int width, int height)
{
    QOpenGLContext *context = QOpenGLContext::currentContext();

    if (!context || !context->isValid() || (width <= 0 || height <= 0))
        return Texture();

    QOpenGLExtraFunctions glF(context);
    glF.initializeOpenGLFunctions();

    // Render to QImage
    QImage image(width, height, QImage::Format_RGBA8888);

    // Clear the image to be fully transparent
    image.fill(Qt::transparent);

    QPainter painter(&image);
    paint(&painter); // Custom paint function
    painter.end();
    image.mirror();

    // Premultiply alpha
    for (int y = 0; y < image.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            QColor color = QColor::fromRgba(line[x]);
            color.setRedF(color.redF() * color.alphaF());
            color.setGreenF(color.greenF() * color.alphaF());
            color.setBlueF(color.blueF() * color.alphaF());
            line[x] = color.rgba();
        }
    }

    // Create final texture from the image
    auto texture = std::make_shared<QOpenGLTexture>(image);
    m_textures.push_back(texture);
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    texture->setMagnificationFilter(QOpenGLTexture::Nearest);
    texture->bind();
    glF.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glF.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    texture->release();

    return Texture(texture->textureId(), width, height);
}
