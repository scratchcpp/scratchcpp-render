// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/costume.h>

#include "penlayer.h"
#include "penlayerpainter.h"
#include "penattributes.h"
#include "irenderedtarget.h"
#include "spritemodel.h"
#include "stagemodel.h"

using namespace scratchcpprender;

static const double pi = std::acos(-1); // TODO: Use std::numbers::pi in C++20

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

    if (m_vao != 0) {
        // Delete vertex array and buffer
        m_glF->glDeleteVertexArrays(1, &m_vao);
        m_glF->glDeleteBuffers(1, &m_vbo);
    }
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

    if (!m_glCtx) {
        m_glCtx = QOpenGLContext::currentContext();

        if (m_glCtx)
            m_surface = m_glCtx->surface();
    }

    if (m_engine && m_glCtx) {
        m_projectPenLayers[m_engine] = this;

        if (!m_painter)
            m_painter = std::make_unique<QNanoPainter>();

        if (!m_glF) {
            m_glF = std::make_unique<QOpenGLExtraFunctions>();
            m_glF->initializeOpenGLFunctions();
        }

        refresh();

        if (m_vao == 0) {
            // Set up VBO and VAO
            float vertices[] = { -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f };

            m_glF->glGenVertexArrays(1, &m_vao);
            m_glF->glGenBuffers(1, &m_vbo);

            m_glF->glBindVertexArray(m_vao);

            m_glF->glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            m_glF->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            // Position attribute
            m_glF->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
            m_glF->glEnableVertexAttribArray(0);

            // Texture coordinate attribute
            m_glF->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
            m_glF->glEnableVertexAttribArray(1);

            m_glF->glBindVertexArray(0);
            m_glF->glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        bool wasBound = m_fbo->isBound();

        if (wasBound)
            beginFrame();

        clear();

        if (wasBound)
            endFrame();
    }

    emit engineChanged();
}

void PenLayer::beginFrame()
{
    m_fbo->bind();
    m_painter->beginFrame(m_fbo->width(), m_fbo->height());
}

void PenLayer::endFrame()
{
    m_painter->endFrame();
    m_fbo->release();
}

bool PenLayer::hqPen() const
{
    return m_hqPen;
}

void PenLayer::setHqPen(bool newHqPen)
{
    if (m_hqPen == newHqPen)
        return;

    m_hqPen = newHqPen;
    emit hqPenChanged();
    refresh();
}

void scratchcpprender::PenLayer::clear()
{
    if (!m_fbo)
        return;

    Q_ASSERT(m_fbo->isBound());
    m_glF->glDisable(GL_SCISSOR_TEST);
    m_glF->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    m_glF->glClear(GL_COLOR_BUFFER_BIT);
    m_glF->glEnable(GL_SCISSOR_TEST);

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

    Q_ASSERT(m_fbo->isBound());

    // Apply scale (HQ pen)
    x0 *= m_scale;
    y0 *= m_scale;
    x1 *= m_scale;
    y1 *= m_scale;

    // Translate to Scratch coordinate system
    double stageWidthHalf = width() / 2;
    double stageHeightHalf = height() / 2;
    x0 += stageWidthHalf;
    y0 = stageHeightHalf - y0;
    x1 += stageWidthHalf;
    y1 = stageHeightHalf - y1;

    // Set pen attributes
    const double diameter = penAttributes.diameter * m_scale;
    m_painter->setLineWidth(diameter);
    m_painter->setStrokeStyle(penAttributes.color);
    m_painter->setFillStyle(penAttributes.color);
    m_painter->setLineJoin(QNanoPainter::JOIN_ROUND);
    m_painter->setLineCap(QNanoPainter::CAP_ROUND);
    m_painter->setAntialias(m_antialiasingEnabled ? 1.0f : 0.0f);
    m_painter->beginPath();

    // Width 1 and 3 lines need to be offset by 0.5
    const double offset = (std::fmod(std::max(4 - diameter, 0.0), 2)) / 2;

    // If the start and end coordinates are the same, draw a point, otherwise draw a line
    if (x0 == x1 && y0 == y1) {
        m_painter->circle(x0 + offset, y0 + offset, diameter / 2);
        m_painter->fill();
    } else {
        m_painter->moveTo(x0 + offset, y0 + offset);
        m_painter->lineTo(x1 + offset, y1 + offset);
        m_painter->stroke();
    }

    m_textureDirty = true;
    m_boundsDirty = true;
    update();
}

void PenLayer::stamp(IRenderedTarget *target)
{
    if (!target || !m_fbo || !m_texture.isValid() || m_vao == 0 || m_vbo == 0)
        return;

    Q_ASSERT(m_fbo->isBound());

    m_glF->glDisable(GL_SCISSOR_TEST);
    m_glF->glDisable(GL_DEPTH_TEST);

    m_glF->glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    m_glF->glBindVertexArray(m_vao);

    target->render(m_scale);

    m_glF->glBindVertexArray(0);
    m_glF->glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_glF->glEnable(GL_SCISSOR_TEST);
    m_glF->glEnable(GL_DEPTH_TEST);

    m_textureDirty = true;
    m_boundsDirty = true;
    update();
}

void PenLayer::refresh()
{
    if (!m_glCtx || !m_surface || !m_engine || !m_glF)
        return;

    QOpenGLContext *oldCtx = QOpenGLContext::currentContext();
    QSurface *oldSurface = oldCtx->surface();

    if (oldCtx != m_glCtx) {
        oldCtx->doneCurrent();
        m_glCtx->makeCurrent(m_surface);
    }

    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    QOpenGLFramebufferObject *newFbo = new QOpenGLFramebufferObject(width(), height(), fboFormat);
    Q_ASSERT(newFbo->isValid());

    if (m_fbo) {
        m_glF->glDisable(GL_SCISSOR_TEST);
        QOpenGLFramebufferObject::blitFramebuffer(newFbo, m_fbo.get());
        m_glF->glEnable(GL_SCISSOR_TEST);
    }

    m_fbo.reset(newFbo);
    m_texture = Texture(m_fbo->texture(), m_fbo->size());
    m_scale = width() / m_engine->stageWidth();

    if (oldCtx != m_glCtx) {
        m_glCtx->doneCurrent();
        oldCtx->makeCurrent(oldSurface);
    }
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

    // Apply scale (HQ pen)
    x *= m_scale;
    y *= m_scale;

    // Translate the coordinates
    x = std::floor(x + width / 2.0);
    y = std::floor(-y + height / 2.0);

    // If the point is outside the texture, return fully transparent color
    if ((x < 0 || x >= width) || (y < 0 || y >= height))
        return qRgba(0, 0, 0, 0);

    bool bound = m_fbo->isBound();

    if (bound)
        const_cast<PenLayer *>(this)->endFrame();

    GLubyte *data = m_textureManager.getTextureData(m_texture);
    const int index = (y * width + x) * 4; // RGBA channels
    Q_ASSERT(index >= 0 && index < width * height * 4);

    if (bound)
        const_cast<PenLayer *>(this)->beginFrame();

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
        std::vector<QPoint> points;

        bool bound = m_fbo->isBound();

        if (bound)
            const_cast<PenLayer *>(this)->endFrame();

        m_textureManager.getTextureConvexHullPoints(m_texture, QSize(), ShaderManager::Effect::NoEffect, {}, points);

        if (bound)
            const_cast<PenLayer *>(this)->beginFrame();

        if (points.empty()) {
            m_bounds = libscratchcpp::Rect();
            return m_bounds;
        }

        for (const QPointF &point : points) {
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

        m_bounds.setLeft(left / m_scale);
        m_bounds.setTop(top / m_scale);
        m_bounds.setRight(right / m_scale + 1);
        m_bounds.setBottom(bottom / m_scale - 1);
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

void PenLayer::removePenLayer(libscratchcpp::IEngine *engine)
{
    m_projectPenLayers.erase(engine);
}

QNanoQuickItemPainter *PenLayer::createItemPainter() const
{
    m_glCtx = QOpenGLContext::currentContext();
    Q_ASSERT(m_glCtx);
    m_surface = m_glCtx->surface();
    Q_ASSERT(m_surface);
    return new PenLayerPainter;
}

void PenLayer::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (m_hqPen && newGeometry != oldGeometry)
        refresh();

    QNanoQuickItem::geometryChange(newGeometry, oldGeometry);
}

void PenLayer::updateTexture()
{
    if (!m_fbo)
        return;

    m_textureDirty = false;
    m_textureManager.removeTexture(m_texture);
}
