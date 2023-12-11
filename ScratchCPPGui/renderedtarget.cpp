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
    IRenderedTarget(parent)
{
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
            m_x = static_cast<double>(m_engine->stageWidth()) / 2 + sprite->x() - m_costume->rotationCenterX() * size / 2 * (m_newMirrorHorizontally ? -1 : 1);
            m_y = static_cast<double>(m_engine->stageHeight()) / 2 - sprite->y() - m_costume->rotationCenterY() * size / 2;
            m_originX = m_costume->rotationCenterX() * size / 2.0;
            m_originY = m_costume->rotationCenterY() * size / 2.0;

            // Layer
            m_z = sprite->layerOrder();
        }

        mutex.unlock();
    } else if (m_stageModel) {
        m_x = static_cast<double>(m_engine->stageWidth()) / 2 - m_costume->rotationCenterX() / 2.0;
        m_y = static_cast<double>(m_engine->stageHeight()) / 2 - m_costume->rotationCenterY() / 2.0;
        m_originX = m_costume->rotationCenterX() / 2.0;
        m_originY = m_costume->rotationCenterY() / 2.0;
    }
}

void RenderedTarget::loadCostume(Costume *costume)
{
    if (!costume)
        return;

    m_costumeMutex.lock();
    m_imageChanged = true;

    if (costume->dataFormat() == "svg") {
    }

    m_costume = costume;
    m_costumeMutex.unlock();
}

void RenderedTarget::updateProperties()
{
    mutex.lock();
    setVisible(m_visible);

    if (m_visible) {
        if (m_imageChanged) {
            doLoadCostume();
            update();
            m_imageChanged = false;
        }

        setX(m_x);
        setY(m_y);
        setZ(m_z);
        setRotation(m_rotation);
        setTransformOriginPoint(QPointF(m_originX, m_originY));

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

void RenderedTarget::doLoadCostume()
{
    m_costumeMutex.lock();

    if (!m_costume) {
        m_costumeMutex.unlock();
        return;
    }

    Target *target = scratchTarget();

    if (m_costume->dataFormat() == "svg") {
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

void RenderedTarget::calculateSize(Target *target, double costumeWidth, double costumeHeight)
{
    if (m_costume) {
        double bitmapRes = m_costume->bitmapResolution();
        Sprite *sprite = dynamic_cast<Sprite *>(target);

        if (sprite) {
            double size = sprite->size();
            setWidth(costumeWidth * size / 100 / bitmapRes);
            setHeight(costumeHeight * size / 100 / bitmapRes);
        } else {
            setWidth(costumeWidth / bitmapRes);
            setHeight(costumeHeight / bitmapRes);
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
