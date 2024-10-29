// SPDX-License-Identifier: LGPL-3.0-or-later

#include "cputexturemanager.h"
#include "texture.h"
#include "effecttransform.h"

using namespace scratchcpprender;

CpuTextureManager::CpuTextureManager()
{
}

CpuTextureManager::~CpuTextureManager()
{
    for (const auto &[handle, data] : m_textureData)
        delete[] data;
}

GLubyte *CpuTextureManager::getTextureData(const Texture &texture)
{
    if (!texture.isValid())
        return nullptr;

    const GLuint handle = texture.handle();
    auto it = m_textureData.find(handle);

    if (it == m_textureData.cend()) {
        if (addTexture(texture))
            return m_textureData[handle];
        else
            return nullptr;
    } else
        return it->second;
}

const std::vector<QPoint> &CpuTextureManager::getTextureConvexHullPoints(const Texture &texture)
{
    static const std::vector<QPoint> empty;

    if (!texture.isValid())
        return empty;

    const GLuint handle = texture.handle();
    auto it = m_convexHullPoints.find(handle);

    if (it == m_convexHullPoints.cend()) {
        if (addTexture(texture))
            return m_convexHullPoints[handle];
        else
            return empty;
    } else
        return it->second;
}

QRgb CpuTextureManager::getPointColor(const Texture &texture, int x, int y, ShaderManager::Effect effectMask, const std::unordered_map<ShaderManager::Effect, double> &effects)
{
    const int width = texture.width();
    const int height = texture.height();

    if (effectMask != 0) {
        // Get local position with effect transform
        QVector2D transformedCoords;
        const QVector2D localCoords(x / static_cast<float>(width), y / static_cast<float>(height));
        EffectTransform::transformPoint(effectMask, effects, localCoords, transformedCoords);
        x = transformedCoords.x() * width;
        y = transformedCoords.y() * height;
    }

    if ((x < 0 || x >= width) || (y < 0 || y >= height))
        return qRgba(0, 0, 0, 0);

    GLubyte *pixels = getTextureData(texture);
    QRgb color = qRgba(pixels[(y * width + x) * 4], pixels[(y * width + x) * 4 + 1], pixels[(y * width + x) * 4 + 2], pixels[(y * width + x) * 4 + 3]);

    if (effectMask == 0)
        return color;
    else
        return EffectTransform::transformColor(effectMask, effects, color);
}

bool CpuTextureManager::textureContainsPoint(const Texture &texture, const QPointF &localPoint, ShaderManager::Effect effectMask, const std::unordered_map<ShaderManager::Effect, double> &effects)
{
    // https://github.com/scratchfoundation/scratch-render/blob/7b823985bc6fe92f572cc3276a8915e550f7c5e6/src/Silhouette.js#L219-L226
    const int width = texture.width();
    const int height = texture.height();
    int x = localPoint.x();
    int y = localPoint.y();

    if (effectMask != 0) {
        // Get local position with effect transform
        QVector2D transformedCoords;
        const QVector2D localCoords(x / static_cast<float>(width), y / static_cast<float>(height));
        EffectTransform::transformPoint(effectMask, effects, localCoords, transformedCoords);
        x = transformedCoords.x() * width;
        y = transformedCoords.y() * height;
    }

    if ((x < 0 || x >= width) || (y < 0 || y >= height))
        return false;

    GLubyte *pixels = getTextureData(texture);
    QRgb color = qRgba(pixels[(y * width + x) * 4], pixels[(y * width + x) * 4 + 1], pixels[(y * width + x) * 4 + 2], pixels[(y * width + x) * 4 + 3]);
    return qAlpha(color) > 0;
}

void CpuTextureManager::removeTexture(const Texture &texture)
{
    if (!texture.isValid())
        return;

    const GLuint handle = texture.handle();
    auto it = m_textureData.find(handle);

    if (it != m_textureData.cend()) {
        delete it->second;
        m_textureData.erase(it);
        m_convexHullPoints.erase(handle);
    }
}

bool CpuTextureManager::addTexture(const Texture &texture)
{
    if (!texture.isValid())
        return false;

    const GLuint handle = texture.handle();
    const int width = texture.width();
    const int height = texture.height();

    QOpenGLFunctions glF;
    glF.initializeOpenGLFunctions();

    // Create a FBO for the texture
    unsigned int fbo;
    glF.glGenFramebuffers(1, &fbo);
    glF.glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glF.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handle, 0);

    if (glF.glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        qWarning() << "error: framebuffer incomplete (CpuTextureManager)";
        glF.glDeleteFramebuffers(1, &fbo);
        return false;
    }

    // Read pixels
    GLubyte *pixels = new GLubyte[width * height * 4]; // 4 channels (RGBA)
    glF.glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Flip vertically
    int rowSize = width * 4;
    GLubyte *tempRow = new GLubyte[rowSize];

    for (size_t i = 0; i < height / 2; ++i) {
        size_t topRowIndex = i * rowSize;
        size_t bottomRowIndex = (height - 1 - i) * rowSize;

        // Swap rows
        memcpy(tempRow, &pixels[topRowIndex], rowSize);
        memcpy(&pixels[topRowIndex], &pixels[bottomRowIndex], rowSize);
        memcpy(&pixels[bottomRowIndex], tempRow, rowSize);
    }

    delete[] tempRow;

    m_textureData[handle] = pixels;
    m_convexHullPoints[handle] = {};
    std::vector<QPoint> &hullPoints = m_convexHullPoints[handle];

    // Get convex hull points
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 4; // 4 channels (RGBA)

            // Check alpha channel
            if (pixels[index + 3] > 0)
                hullPoints.push_back(QPoint(x, y));
        }
    }

    // Cleanup
    glF.glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glF.glDeleteFramebuffers(1, &fbo);

    return true;
}
