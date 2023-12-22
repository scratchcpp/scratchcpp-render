// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/iengine.h>
#include <scratchcpp/costume.h>
#include <QtSvg/QSvgRenderer>
#include <qnanopainter.h>

#include "renderedtarget.h"
#include "targetpainter.h"
#include "stagemodel.h"
#include "spritemodel.h"

using namespace scratchcppgui;
using namespace libscratchcpp;

static const double SVG_SCALE_LIMIT = 0.1; // the maximum viewport dimensions are multiplied by this

RenderedTarget::RenderedTarget(QNanoQuickItem *parent) :
    IRenderedTarget(parent)
{
    // Get maximum viewport dimensions
    QOpenGLContext context;
    context.create();
    Q_ASSERT(context.isValid());

    if (context.isValid()) {
        QOffscreenSurface surface;
        surface.create();
        Q_ASSERT(surface.isValid());

        if (surface.isValid()) {
            context.makeCurrent(&surface);
            GLint dims[2];
            glGetIntegerv(GL_MAX_VIEWPORT_DIMS, dims);
            m_maximumWidth = dims[0] * SVG_SCALE_LIMIT;
            m_maximumHeight = dims[1] * SVG_SCALE_LIMIT;
            context.doneCurrent();
        }
    }
}

void RenderedTarget::loadProperties()
{
    Q_ASSERT(!(m_spriteModel && m_stageModel));
    if (!m_engine || !m_costume)
        return;

    if (m_spriteModel) {
        mutex.lock();
        Sprite *sprite = m_spriteModel->sprite();

        // Visibility
        m_visible = sprite->visible();

        m_size = sprite->size() / 100;
        updateCostumeData();

        if (m_visible) {
            // Direction
            switch (sprite->rotationStyle()) {
                case Sprite::RotationStyle::AllAround:
                    m_rotation = sprite->direction() - 90;
                    m_newMirrorHorizontally = false;

                    break;

                case Sprite::RotationStyle::LeftRight: {
                    m_rotation = 0;
                    m_newMirrorHorizontally = (sprite->direction() < 0);

                    break;
                }

                case Sprite::RotationStyle::DoNotRotate:
                    m_rotation = 0;
                    m_newMirrorHorizontally = false;
                    break;
            }

            // Coordinates
            double clampedSize = std::min(m_size, m_maxSize);
            m_x = static_cast<double>(m_engine->stageWidth()) / 2 + sprite->x() - m_costume->rotationCenterX() * clampedSize / m_costume->bitmapResolution() * (m_newMirrorHorizontally ? -1 : 1);
            m_y = static_cast<double>(m_engine->stageHeight()) / 2 - sprite->y() - m_costume->rotationCenterY() * clampedSize / m_costume->bitmapResolution();
            m_originX = m_costume->rotationCenterX() * clampedSize / m_costume->bitmapResolution();
            m_originY = m_costume->rotationCenterY() * clampedSize / m_costume->bitmapResolution();

            // Layer
            m_z = sprite->layerOrder();
        }

        mutex.unlock();
    } else if (m_stageModel) {
        updateCostumeData();
        m_x = static_cast<double>(m_engine->stageWidth()) / 2 - m_costume->rotationCenterX() / m_costume->bitmapResolution();
        m_y = static_cast<double>(m_engine->stageHeight()) / 2 - m_costume->rotationCenterY() / m_costume->bitmapResolution();
        m_originX = m_costume->rotationCenterX() / m_costume->bitmapResolution();
        m_originY = m_costume->rotationCenterY() / m_costume->bitmapResolution();
    }
}

void RenderedTarget::loadCostume(Costume *costume)
{
    if (!costume)
        return;

    m_costumeMutex.lock();
    m_loadCostume = true;
    m_costumeChanged = (costume != m_costume);
    m_costume = costume;
    m_costumeMutex.unlock();
}

void RenderedTarget::updateProperties()
{
    mutex.lock();
    setVisible(m_visible);

    if (m_visible) {
        if (m_imageChanged) {
            update();
            m_imageChanged = false;
        }

        setX(m_x);
        setY(m_y);
        setZ(m_z);
        setWidth(m_width);
        setHeight(m_height);
        setRotation(m_rotation);
        setTransformOriginPoint(QPointF(m_originX, m_originY));
        Q_ASSERT(m_maxSize > 0);

        if (!m_stageModel && (m_size > m_maxSize) && (m_maxSize != 0))
            setScale(m_size / m_maxSize);
        else
            setScale(1);

        if (m_newMirrorHorizontally != m_mirrorHorizontally) {
            m_mirrorHorizontally = m_newMirrorHorizontally;
            emit mirrorHorizontallyChanged();
        }
    }

    mutex.unlock();
}

IEngine *RenderedTarget::engine() const
{
    return m_engine;
}

void RenderedTarget::setEngine(IEngine *newEngine)
{
    if (m_engine == newEngine)
        return;

    m_engine = newEngine;
    emit engineChanged();
}

StageModel *RenderedTarget::stageModel() const
{
    return m_stageModel;
}

void RenderedTarget::setStageModel(StageModel *newStageModel)
{
    if (m_stageModel == newStageModel)
        return;

    m_stageModel = newStageModel;
    emit stageModelChanged();
}

SpriteModel *RenderedTarget::spriteModel() const
{
    return m_spriteModel;
}

void RenderedTarget::setSpriteModel(SpriteModel *newSpriteModel)
{
    if (m_spriteModel == newSpriteModel)
        return;

    m_spriteModel = newSpriteModel;
    emit spriteModelChanged();
}

Target *RenderedTarget::scratchTarget() const
{
    if (m_spriteModel)
        return m_spriteModel->sprite();
    else if (m_stageModel)
        return m_stageModel->stage();
    else
        return nullptr;
}

qreal RenderedTarget::width() const
{
    return QNanoQuickItem::width();
}

void RenderedTarget::setWidth(qreal width)
{
    QNanoQuickItem::setWidth(width);
}

qreal RenderedTarget::height() const
{
    return QNanoQuickItem::height();
}

void RenderedTarget::setHeight(qreal height)
{
    QNanoQuickItem::setHeight(height);
}

QNanoQuickItemPainter *RenderedTarget::createItemPainter() const
{
    return new TargetPainter();
}

void RenderedTarget::updateCostumeData()
{
    // Costume
    m_costumeMutex.lock();

    if (m_loadCostume) {
        m_loadCostume = false;
        m_imageChanged = true;

        if (m_costumeChanged) {
            m_costumeChanged = false;
            assert(m_costume);

            if (m_costume->dataFormat() == "svg")
                m_svgRenderer.load(QByteArray::fromRawData(static_cast<const char *>(m_costume->data()), m_costume->dataSize()));
        }
    }

    m_costumeMutex.unlock();
    doLoadCostume();
}

void RenderedTarget::doLoadCostume()
{
    m_costumeMutex.lock();

    if (!m_costume) {
        m_costumeMutex.unlock();
        return;
    }

    Target *target = scratchTarget();

    if (m_costume->dataFormat() == "svg") {
        QRectF rect = m_svgRenderer.viewBoxF();
        calculateSize(target, rect.width(), rect.height());
    } else {
        m_bitmapBuffer.open(QBuffer::WriteOnly);
        m_bitmapBuffer.write(static_cast<const char *>(m_costume->data()), m_costume->dataSize());
        m_bitmapBuffer.close();
        m_bitmapUniqueKey = QString::fromStdString(m_costume->id());

        QImageReader reader(&m_bitmapBuffer);
        QSize size = reader.size();
        calculateSize(target, size.width(), size.height());
        m_bitmapBuffer.close();
    }

    m_costumeMutex.unlock();
}

void RenderedTarget::paintSvg(QNanoPainter *painter)
{
    Q_ASSERT(painter);
    QOpenGLContext *context = QOpenGLContext::currentContext();
    Q_ASSERT(context);

    if (!context)
        return;

    QOffscreenSurface surface;
    surface.setFormat(context->format());
    surface.create();
    Q_ASSERT(surface.isValid());

    QSurface *oldSurface = context->surface();
    context->makeCurrent(&surface);

    const QRectF drawRect(0, 0, std::min(width(), m_maximumWidth), std::min(height(), m_maximumHeight));
    const QSize drawRectSize = drawRect.size().toSize();

    /*QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setSamples(16);
    fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);*/

    QOpenGLPaintDevice device(drawRectSize);
    QPainter qPainter;
    qPainter.begin(&device);
    qPainter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    m_svgRenderer.render(&qPainter, drawRect);
    qPainter.end();

    context->doneCurrent();
    context->makeCurrent(oldSurface);
}

void RenderedTarget::calculateSize(Target *target, double costumeWidth, double costumeHeight)
{
    if (m_costume) {
        double bitmapRes = m_costume->bitmapResolution();
        m_maxSize = std::min(m_maximumWidth / costumeWidth, m_maximumHeight / costumeHeight);
        Sprite *sprite = dynamic_cast<Sprite *>(target);

        if (sprite) {
            double clampedSize = std::min(m_size, m_maxSize);
            m_width = costumeWidth * clampedSize / bitmapRes;
            m_height = costumeHeight * clampedSize / bitmapRes;
        } else {
            m_width = costumeWidth / bitmapRes;
            m_height = costumeHeight / bitmapRes;
        }
    }
}

QBuffer *RenderedTarget::bitmapBuffer()
{
    return &m_bitmapBuffer;
}

const QString &RenderedTarget::bitmapUniqueKey() const
{
    return m_bitmapUniqueKey;
}

void RenderedTarget::lockCostume()
{
    m_costumeMutex.lock();
}

void RenderedTarget::unlockCostume()
{
    m_costumeMutex.unlock();
}

bool RenderedTarget::mirrorHorizontally() const
{
    return m_mirrorHorizontally;
}

bool RenderedTarget::isSvg() const
{
    if (!m_costume)
        return false;

    return (m_costume->dataFormat() == "svg");
}
