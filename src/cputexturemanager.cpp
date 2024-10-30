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

void CpuTextureManager::getTextureConvexHullPoints(
    const Texture &texture,
    const QSize &skinSize,
    ShaderManager::Effect effectMask,
    const std::unordered_map<ShaderManager::Effect, double> &effects,
    std::vector<QPoint> &dst)
{
    dst.clear();

    if (!texture.isValid())
        return;

    // Remove effects that don't change shape
    if (effectMask != 0) {
        const auto &allEffects = ShaderManager::effects();

        for (ShaderManager::Effect effect : allEffects) {
            if ((effectMask & effect) != 0 && !ShaderManager::effectShapeChanges(effect))
                effectMask &= ~effect;
        }
    }

    // If there are no shape-changing effects, use cached hull points
    if (effectMask == 0) {
        const GLuint handle = texture.handle();
        auto it = m_convexHullPoints.find(handle);

        if (it == m_convexHullPoints.cend()) {
            if (addTexture(texture))
                dst = m_convexHullPoints[handle];
        } else
            dst = it->second;
    } else
        readTexture(texture, skinSize, effectMask, effects, nullptr, dst);
}

QRgb CpuTextureManager::getPointColor(const Texture &texture, int x, int y, ShaderManager::Effect effectMask, const std::unordered_map<ShaderManager::Effect, double> &effects)
{
    const int width = texture.width();
    const int height = texture.height();

    if (effectMask != 0) {
        // Get local position with effect transform
        QVector2D transformedCoords;
        const QVector2D localCoords(x / static_cast<float>(width), y / static_cast<float>(height));
        EffectTransform::transformPoint(effectMask, effects, texture.size(), localCoords, transformedCoords);
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
        EffectTransform::transformPoint(effectMask, effects, texture.size(), localCoords, transformedCoords);
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

bool CpuTextureManager::addTexture(const Texture &tex)
{
    if (!tex.isValid())
        return false;

    const GLuint handle = tex.handle();
    m_textureData[handle] = nullptr;
    m_convexHullPoints[handle] = {};
    return readTexture(tex, QSize(), ShaderManager::Effect::NoEffect, {}, &m_textureData[handle], m_convexHullPoints[handle]);
}

bool CpuTextureManager::readTexture(
    const Texture &texture,
    const QSize &skinSize,
    ShaderManager::Effect effectMask,
    const std::unordered_map<ShaderManager::Effect, double> &effects,
    GLubyte **data,
    std::vector<QPoint> &points) const
{
    if (!texture.isValid())
        return false;

    const GLuint handle = texture.handle();
    const int width = texture.width();
    const int height = texture.height();

    QOpenGLFunctions glF;
    glF.initializeOpenGLFunctions();

    // Create global FBO
    if (m_fbo == 0) {
        glF.glGenFramebuffers(1, &m_fbo);

        QObject::connect(QOpenGLContext::currentContext(), &QOpenGLContext::aboutToBeDestroyed, []() {
            if (QOpenGLContext::currentContext()) {
                QOpenGLFunctions glF;
                glF.initializeOpenGLFunctions();
                glF.glDeleteFramebuffers(1, &m_fbo);
                m_fbo = 0;
            }
        });
    }

    // Bind the texture to the global FBO
    glF.glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glF.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handle, 0);

    if (glF.glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        qWarning() << "error: framebuffer incomplete (CpuTextureManager)";
        glF.glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    // Read pixels
    GLubyte *pixels = new GLubyte[width * height * 4]; // 4 channels (RGBA)
    glF.glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    std::vector<QPoint> leftHull;
    std::vector<QPoint> rightHull;
    leftHull.reserve(height);
    rightHull.reserve(height);

    for (int x = 0; x < height; x++) {
        leftHull.push_back(QPoint(-1, -1));
        rightHull.push_back(QPoint(-1, -1));
    }

    int leftEndPointIndex = -1;
    int rightEndPointIndex = -1;

    auto determinant = [](const QPoint &A, const QPoint &B, const QPoint &C) { return (B.x() - A.x()) * (C.y() - A.y()) - (B.y() - A.y()) * (C.x() - A.x()); };

    // Get convex hull points (flipped vertically)
    // https://github.com/scratchfoundation/scratch-render/blob/0f6663f3148b4f994d58e19590e14c152f1cc2f8/src/RenderWebGL.js#L1829-L1955
    for (int y = 0; y < height; y++) {
        QPoint currentPoint;
        int x;
        const int flippedY = height - 1 - y;

        for (x = 0; x < width; x++) {
            int transformedX = x;
            int transformedY = flippedY;

            if (effectMask != 0) {
                // Get local position with effect transform
                QVector2D transformedCoords;
                const QVector2D localCoords(transformedX / static_cast<float>(width), transformedY / static_cast<float>(height));
                EffectTransform::transformPoint(effectMask, effects, skinSize, localCoords, transformedCoords);
                transformedX = transformedCoords.x() * width;
                transformedY = transformedCoords.y() * height;
            }

            if ((transformedX >= 0 && transformedX < width) && (transformedY >= 0 && transformedY < height)) {
                int index = (transformedY * width + transformedX) * 4;

                if (pixels[index + 3] > 0) {
                    currentPoint.setX(x);
                    currentPoint.setY(y);
                    break;
                }
            }
        }

        if (x >= width)
            continue;

        while (leftEndPointIndex > 0) {
            if (determinant(leftHull[leftEndPointIndex], leftHull[leftEndPointIndex - 1], currentPoint) > 0)
                break;
            else {
                leftEndPointIndex--;
            }
        }

        leftHull[++leftEndPointIndex] = currentPoint;

        for (x = width - 1; x >= 0; x--) {
            int transformedX = x;
            int transformedY = flippedY;

            if (effectMask != 0) {
                // Get local position with effect transform
                QVector2D transformedCoords;
                const QVector2D localCoords(transformedX / static_cast<float>(width), transformedY / static_cast<float>(height));
                EffectTransform::transformPoint(effectMask, effects, skinSize, localCoords, transformedCoords);
                transformedX = transformedCoords.x() * width;
                transformedY = transformedCoords.y() * height;
            }

            if ((transformedX >= 0 && transformedX < width) && (transformedY >= 0 && transformedY < height)) {
                int index = (transformedY * width + transformedX) * 4;

                if (pixels[index + 3] > 0) {
                    currentPoint.setX(x);
                    currentPoint.setY(y);
                    break;
                }
            }
        }

        while (rightEndPointIndex > 0) {
            if (determinant(rightHull[rightEndPointIndex], rightHull[rightEndPointIndex - 1], currentPoint) < 0)
                break;
            else
                rightEndPointIndex--;
        }

        rightHull[++rightEndPointIndex] = currentPoint;
    }

    points.clear();
    points.reserve((leftEndPointIndex + 1) + (rightEndPointIndex + 1));

    long i;

    for (i = 0; i < leftHull.size(); i++) {
        if (leftHull[i].x() >= 0)
            points.push_back(leftHull[i]);
    }

    for (i = rightEndPointIndex; i >= 0; --i)
        if (rightHull[i].x() >= 0)
            points.push_back(rightHull[i]);

    if (data) {
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

        *data = pixels;
    } else
        delete[] pixels;

    // Cleanup
    glF.glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}
