// SPDX-License-Identifier: LGPL-3.0-or-later

#include "penlayer.h"
#include "penlayerpainter.h"
#include "penattributes.h"

using namespace scratchcpprender;

std::unordered_map<libscratchcpp::IEngine *, IPenLayer *> PenLayer::m_projectPenLayers;

PenLayer::PenLayer(QNanoQuickItem *parent) :
    IPenLayer(parent)
{
    m_fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    m_fboFormat.setSamples(4);
}

PenLayer::~PenLayer()
{
    if (m_engine)
        m_projectPenLayers.erase(m_engine);

    if (m_painter && m_painter->isActive())
        m_painter->end();
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

    if (m_engine) {
        m_projectPenLayers[m_engine] = this;
        m_fbo = std::make_unique<QOpenGLFramebufferObject>(m_engine->stageWidth(), m_engine->stageHeight(), m_fboFormat);
        Q_ASSERT(m_fbo->isValid());

        if (m_painter && m_painter->isActive())
            m_painter->end();

        m_paintDevice = std::make_unique<QOpenGLPaintDevice>(m_fbo->size());
        m_painter = std::make_unique<QPainter>(m_paintDevice.get());
        clear();
    }

    emit engineChanged();
}

void scratchcpprender::PenLayer::clear()
{
    if (!m_fbo)
        return;

    m_fbo->bind();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    m_fbo->release();

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
    m_painter->beginNativePainting();
    m_painter->setRenderHint(QPainter::Antialiasing);
    m_painter->setRenderHint(QPainter::SmoothPixmapTransform, false);

    // Translate to Scratch coordinate system
    double stageWidthHalf = m_engine->stageWidth() / 2;
    double stageHeightHalf = m_engine->stageHeight() / 2;
    x0 += stageWidthHalf;
    y0 = stageHeightHalf - y0;
    x1 += stageWidthHalf;
    y1 = stageHeightHalf - y1;

    // Set pen attributes
    QPen pen(penAttributes.color);
    pen.setWidthF(penAttributes.diameter);
    pen.setCapStyle(Qt::RoundCap);
    m_painter->setPen(pen);

    // If the start and end coordinates are the same, draw a point, otherwise draw a line
    if (x0 == x1 && y0 == y1)
        m_painter->drawPoint(x0, y0);
    else
        m_painter->drawLine(x0, y0, x1, y1);

    // End painting
    m_painter->endNativePainting();
    m_fbo->release();

    update();
}

QOpenGLFramebufferObject *PenLayer::framebufferObject() const
{
    return m_fbo.get();
}

IPenLayer *PenLayer::getProjectPenLayer(libscratchcpp::IEngine *engine)
{
    auto it = m_projectPenLayers.find(engine);

    if (it != m_projectPenLayers.cend())
        return it->second;

    return nullptr;
}

QNanoQuickItemPainter *PenLayer::createItemPainter() const
{
    return new PenLayerPainter;
}
