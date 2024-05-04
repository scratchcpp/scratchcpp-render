// SPDX-License-Identifier: LGPL-3.0-or-later

#include "penlayer.h"
#include "penlayerpainter.h"
#include "penattributes.h"

using namespace scratchcpprender;

std::unordered_map<libscratchcpp::IEngine *, IPenLayer *> PenLayer::m_projectPenLayers;

PenLayer::PenLayer(QNanoQuickItem *parent) :
    IPenLayer(parent)
{
    setSmooth(false);
}

PenLayer::~PenLayer()
{
    if (m_engine)
        m_projectPenLayers.erase(m_engine);
}

bool PenLayer::antialiasingEnabled() const
{
    return m_antialiasingEnabled;
}

void PenLayer::setAntialiasingEnabled(bool enabled)
{
    m_antialiasingEnabled = enabled;
}

libscratchcpp::IEngine *PenLayer::engine() const
{
    return m_engine;
}

void PenLayer::setEngine(libscratchcpp::IEngine *newEngine)
{
    if (m_engine == newEngine)
        return;

    if (m_engine)
        m_projectPenLayers.erase(m_engine);

    m_engine = newEngine;

    if (m_engine && QOpenGLContext::currentContext()) {
        m_projectPenLayers[m_engine] = this;
        QOpenGLFramebufferObjectFormat fboFormat;
        fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        m_fbo = std::make_unique<QOpenGLFramebufferObject>(m_engine->stageWidth(), m_engine->stageHeight(), fboFormat);
        Q_ASSERT(m_fbo->isValid());
        m_texture = Texture(m_fbo->texture(), m_fbo->size());

        if (!m_painter)
            m_painter = std::make_unique<QNanoPainter>();

        clear();
    }

    emit engineChanged();
}

void scratchcpprender::PenLayer::clear()
{
    if (!m_fbo)
        return;

    if (!m_glF) {
        m_glF = std::make_unique<QOpenGLFunctions>();
        m_glF->initializeOpenGLFunctions();
    }

    m_fbo->bind();
    m_glF->glDisable(GL_SCISSOR_TEST);
    m_glF->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    m_glF->glClear(GL_COLOR_BUFFER_BIT);
    m_glF->glEnable(GL_SCISSOR_TEST);
    m_fbo->release();

    m_textureDirty = true;
    m_boundsDirty = true;
    update();
}

void scratchcpprender::PenLayer::drawPoint(const PenAttributes &penAttributes, double x, double y)
{
    drawLine(penAttributes, x, y, x, y);
}

void scratchcpprender::PenLayer::drawLine(const PenAttributes &penAttributes, double x0, double y0, double x1, double y1)
{
    if (!m_fbo || !m_painter || !m_engine)
        return;

    // Begin painting
    m_fbo->bind();

    m_painter->beginFrame(m_fbo->width(), m_fbo->height());

    // Translate to Scratch coordinate system
    double stageWidthHalf = m_engine->stageWidth() / 2;
    double stageHeightHalf = m_engine->stageHeight() / 2;
    x0 += stageWidthHalf;
    y0 = stageHeightHalf - y0;
    x1 += stageWidthHalf;
    y1 = stageHeightHalf - y1;

    // Set pen attributes
    m_painter->setLineWidth(penAttributes.diameter);
    m_painter->setStrokeStyle(penAttributes.color);
    m_painter->setFillStyle(penAttributes.color);
    m_painter->setLineJoin(QNanoPainter::JOIN_ROUND);
    m_painter->setLineCap(QNanoPainter::CAP_ROUND);
    m_painter->setAntialias(m_antialiasingEnabled ? 1.0f : 0.0f);
    m_painter->beginPath();

    // Width 1 and 3 lines need to be offset by 0.5
    const double offset = (std::fmod(std::max(4 - penAttributes.diameter, 0.0), 2)) / 2;

    // If the start and end coordinates are the same, draw a point, otherwise draw a line
    if (x0 == x1 && y0 == y1) {
        m_painter->circle(x0 + offset, y0 + offset, penAttributes.diameter / 2);
        m_painter->fill();
    } else {
        m_painter->moveTo(x0 + offset, y0 + offset);
        m_painter->lineTo(x1 + offset, y1 + offset);
        m_painter->stroke();
    }

    // End painting
    m_painter->endFrame();
    m_fbo->release();

    m_textureDirty = true;
    m_boundsDirty = true;
    update();
}

QOpenGLFramebufferObject *PenLayer::framebufferObject() const
{
    return m_fbo.get();
}

QRgb PenLayer::colorAtScratchPoint(double x, double y) const
{
    if (m_textureDirty)
        const_cast<PenLayer *>(this)->updateTexture();

    if (!m_texture.isValid())
        return qRgba(0, 0, 0, 0);

    const double width = m_texture.width();
    const double height = m_texture.height();

    // Translate the coordinates
    // TODO: Apply scale
    x = std::floor(x + width / 2.0);
    y = std::floor(-y + height / 2.0);

    // If the point is outside the texture, return fully transparent color
    if ((x < 0 || x >= width) || (y < 0 || y >= height))
        return qRgba(0, 0, 0, 0);

    GLubyte *data = m_textureManager.getTextureData(m_texture);
    const int index = (y * width + x) * 4; // RGBA channels
    Q_ASSERT(index >= 0 && index < width * height * 4);
    return qRgba(data[index], data[index + 1], data[index + 2], data[index + 3]);
}

const libscratchcpp::Rect &PenLayer::getBounds() const
{
    if (m_textureDirty)
        const_cast<PenLayer *>(this)->updateTexture();

    if (m_boundsDirty) {
        if (!m_texture.isValid()) {
            m_bounds = libscratchcpp::Rect();
            return m_bounds;
        }

        m_boundsDirty = false;
        double left = std::numeric_limits<double>::infinity();
        double top = -std::numeric_limits<double>::infinity();
        double right = -std::numeric_limits<double>::infinity();
        double bottom = std::numeric_limits<double>::infinity();
        const double width = m_texture.width();
        const double height = m_texture.height();
        const std::vector<QPoint> &points = m_textureManager.getTextureConvexHullPoints(m_texture);

        if (points.empty()) {
            m_bounds = libscratchcpp::Rect();
            return m_bounds;
        }

        for (const QPointF &point : points) {
            // TODO: Apply scale
            double x = point.x() - width / 2;
            double y = -point.y() + height / 2;

            if (x < left)
                left = x;

            if (x > right)
                right = x;

            if (y > top)
                top = y;

            if (y < bottom)
                bottom = y;
        }

        m_bounds.setLeft(left);
        m_bounds.setTop(top);
        m_bounds.setRight(right + 1);
        m_bounds.setBottom(bottom - 1);
    }

    return m_bounds;
}

IPenLayer *PenLayer::getProjectPenLayer(libscratchcpp::IEngine *engine)
{
    auto it = m_projectPenLayers.find(engine);

    if (it != m_projectPenLayers.cend())
        return it->second;

    return nullptr;
}

void PenLayer::addPenLayer(libscratchcpp::IEngine *engine, IPenLayer *penLayer)
{
    m_projectPenLayers[engine] = penLayer;
}

QNanoQuickItemPainter *PenLayer::createItemPainter() const
{
    return new PenLayerPainter;
}

void PenLayer::updateTexture()
{
    if (!m_fbo)
        return;

    m_textureDirty = false;
    m_textureManager.removeTexture(m_texture);
}
