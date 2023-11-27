// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/iengine.h>
#include <scratchcpp/costume.h>

#include "renderedtarget.h"
#include "targetpainter.h"
#include "stagemodel.h"
#include "spritemodel.h"

using namespace scratchcppgui;
using namespace libscratchcpp;

RenderedTarget::RenderedTarget(QNanoQuickItem *parent) :
    QNanoQuickItem(parent)
{
}

RenderedTarget::~RenderedTarget()
{
    if (m_svgBitmap)
        free(m_svgBitmap);
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
            double size = sprite->size() / 100;
            m_x = m_engine->stageWidth() / 2 + sprite->x() - m_costume->rotationCenterX() * size / 2 * (m_newMirrorHorizontally ? -1 : 1);
            m_y = m_engine->stageHeight() / 2 - sprite->y() - m_costume->rotationCenterY() * size / 2;
            m_originX = m_costume->rotationCenterX() * size / 2.0;
            m_originY = m_costume->rotationCenterY() * size / 2.0;

            // Layer
            m_z = sprite->layerOrder();
        }

        mutex.unlock();
    } else if (m_stageModel) {
        m_x = m_engine->stageWidth() / 2 - m_costume->rotationCenterX() / 2;
        m_y = m_engine->stageHeight() / 2 - m_costume->rotationCenterY() / 2;
        m_originX = m_costume->rotationCenterX() / 2.0;
        m_originY = m_costume->rotationCenterY() / 2.0;
    }
}

void RenderedTarget::loadCostume(Costume *costume)
{
    if (!costume)
        return;

    m_costumeMutex.lock();
    Target *target = scratchTarget();
    m_costume = costume;
    m_imageChanged = true;

    if (costume->dataFormat() == "svg") {
        // TODO: Load SVG here
        // In case of rasterizing, write the bitmap to m_svgBitmap
    } else {
        if (m_svgBitmap) {
            free(m_svgBitmap);
            m_svgBitmap = nullptr;
        }

        m_bitmapBuffer.open(QBuffer::WriteOnly);
        m_bitmapBuffer.write(static_cast<const char *>(costume->data()), costume->dataSize());
        m_bitmapBuffer.close();
        m_bitmapUniqueKey = QString::fromStdString(costume->id());

        QImageReader reader(&m_bitmapBuffer);
        QSize size = reader.size();
        calculateSize(target, size.width(), size.height());
    }

    m_costumeMutex.unlock();
}

void RenderedTarget::updateProperties()
{
    mutex.lock();
    setVisible(m_visible);

    if (m_visible) {
        setWidth(m_width);
        setHeight(m_height);
        setX(m_x);
        setY(m_y);
        setZ(m_z);
        setRotation(m_rotation);
        setTransformOriginPoint(QPointF(m_originX, m_originY));

        if (m_newMirrorHorizontally != m_mirrorHorizontally) {
            m_mirrorHorizontally = m_newMirrorHorizontally;
            emit mirrorHorizontallyChanged();
        }

        if (m_imageChanged) {
            update();
            m_imageChanged = false;
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

unsigned char *RenderedTarget::svgBitmap() const
{
    return m_svgBitmap;
}

QNanoQuickItemPainter *RenderedTarget::createItemPainter() const
{
    return new TargetPainter();
}

void RenderedTarget::calculateSize(Target *target, double costumeWidth, double costumeHeight)
{
    if (m_costume) {
        double bitmapRes = m_costume->bitmapResolution();
        Sprite *sprite = dynamic_cast<Sprite *>(target);

        if (sprite) {
            double size = sprite->size();
            m_width = costumeWidth * size / 100 / bitmapRes;
            m_height = costumeHeight * size / 100 / bitmapRes;
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
