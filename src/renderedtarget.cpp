// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/iengine.h>
#include <scratchcpp/costume.h>
#include <QtSvg/QSvgRenderer>
#include <qnanopainter.h>

#include "renderedtarget.h"
#include "targetpainter.h"
#include "stagemodel.h"
#include "spritemodel.h"
#include "scenemousearea.h"
#include "bitmapskin.h"
#include "svgskin.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

static const double SVG_SCALE_LIMIT = 0.1; // the maximum viewport dimensions are multiplied by this

RenderedTarget::RenderedTarget(QNanoQuickItem *parent) :
    IRenderedTarget(parent)
{
}

RenderedTarget::~RenderedTarget()
{
    if (!m_skinsInherited) {
        for (const auto &[costume, skin] : m_skins)
            delete skin;
    }
}

void RenderedTarget::updateVisibility(bool visible)
{
    if (visible == isVisible())
        return;

    setVisible(visible);
    calculatePos();
}

void RenderedTarget::updateX(double x)
{
    if (x == m_x)
        return;

    m_x = x;
    calculatePos();
}

void RenderedTarget::updateY(double y)
{
    if (y == m_y)
        return;

    m_y = y;
    calculatePos();
}

void RenderedTarget::updateSize(double size)
{
    if (size / 100 == m_size)
        return;

    m_size = size / 100;
    calculateSize();
    calculatePos();
}

void RenderedTarget::updateDirection(double direction)
{
    if (direction == m_direction)
        return;

    m_direction = direction;
    calculateRotation();

    if (m_rotationStyle == libscratchcpp::Sprite::RotationStyle::LeftRight)
        calculatePos();
}

void RenderedTarget::updateRotationStyle(libscratchcpp::Sprite::RotationStyle style)
{
    if (style == m_rotationStyle)
        return;

    m_rotationStyle = style;
    calculateRotation();
    calculatePos();
}

void RenderedTarget::updateLayerOrder(int layerOrder)
{
    setZ(layerOrder);
}

void RenderedTarget::updateCostume(Costume *costume)
{
    if (!costume || costume == m_costume)
        return;

    m_costume = costume;

    if (m_costumesLoaded) {
        auto it = m_skins.find(m_costume);

        if (it == m_skins.end())
            m_skin = nullptr;
        else
            m_skin = it->second;
    }

    calculateSize();
    calculatePos();
}

bool RenderedTarget::costumesLoaded() const
{
    return m_costumesLoaded;
}

void RenderedTarget::loadCostumes()
{
    // Delete previous skins
    if (!m_skinsInherited) {
        for (const auto &[costume, skin] : m_skins)
            delete skin;
    }

    m_skinsInherited = false;
    m_skins.clear();

    // Generate a skin for each costume
    Target *target = scratchTarget();

    if (!target)
        return;

    const auto &costumes = target->costumes();

    for (auto costume : costumes) {
        Skin *skin = nullptr;
        if (costume->dataFormat() == "svg")
            skin = new SVGSkin(costume.get());
        else
            skin = new BitmapSkin(costume.get());

        if (skin)
            m_skins[costume.get()] = skin;

        if (m_costume && costume.get() == m_costume)
            m_skin = skin;
    }

    m_costumesLoaded = true;

    if (m_costume) {
        calculateSize();
        calculatePos();
    }
}

void RenderedTarget::beforeRedraw()
{
    // These properties must be set here to avoid unnecessary calls to update()
    setWidth(m_width);
    setHeight(m_height);

    if (!m_oldTexture.isValid() || (m_texture.isValid() && m_texture != m_oldTexture)) {
        m_oldTexture = m_texture;
        update();
    }
}

void RenderedTarget::deinitClone()
{
    // Do not process mouse move events after the clone has been deleted
    disconnect(m_mouseArea, &SceneMouseArea::mouseMoved, this, &RenderedTarget::handleSceneMouseMove);

    // Release drag lock
    if (m_mouseArea->draggedSprite() == this)
        m_mouseArea->setDraggedSprite(nullptr);
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

    if (m_stageModel) {
        Stage *stage = m_stageModel->stage();

        if (stage)
            updateCostume(stage->currentCostume().get());
    }

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

    if (m_spriteModel) {
        SpriteModel *cloneRoot = m_spriteModel->cloneRoot();

        if (cloneRoot) {
            // Inherit skins from the clone root
            RenderedTarget *target = dynamic_cast<RenderedTarget *>(cloneRoot->renderedTarget());
            Q_ASSERT(target);

            if (target->costumesLoaded()) {
                m_skins = target->m_skins; // TODO: Avoid copying - maybe using a pointer?
                m_costumesLoaded = true;
                m_skinsInherited = true; // avoid double free
            }
        }

        Sprite *sprite = m_spriteModel->sprite();

        if (sprite) {
            m_x = sprite->x();
            m_y = sprite->y();
            m_size = sprite->size() / 100;
            m_direction = sprite->direction();
            m_rotationStyle = sprite->rotationStyle();
            updateCostume(sprite->currentCostume().get());
            updateVisibility(sprite->visible());
            updateLayerOrder(sprite->layerOrder());
            calculateSize();
            calculatePos();
            calculateRotation();
        }
    }
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

SceneMouseArea *RenderedTarget::mouseArea() const
{
    return m_mouseArea;
}

void RenderedTarget::setMouseArea(SceneMouseArea *newMouseArea)
{
    if (m_mouseArea == newMouseArea)
        return;

    m_mouseArea = newMouseArea;
    Q_ASSERT(m_mouseArea);
    connect(m_mouseArea, &SceneMouseArea::mouseMoved, this, &RenderedTarget::handleSceneMouseMove);
    emit mouseAreaChanged();
}

double RenderedTarget::stageScale() const
{
    return m_stageScale;
}

void RenderedTarget::setStageScale(double newStageScale)
{
    if (qFuzzyCompare(m_stageScale, newStageScale))
        return;

    m_stageScale = newStageScale;
    calculateSize();
    calculatePos();
    emit stageScaleChanged();
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

QPointF RenderedTarget::mapFromScene(const QPointF &point) const
{
    return QNanoQuickItem::mapFromScene(point);
}

QNanoQuickItemPainter *RenderedTarget::createItemPainter() const
{
    return new TargetPainter();
}

void RenderedTarget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_clicked = true;

    if (m_engine && (!m_spriteModel || !m_spriteModel->sprite()->draggable())) {
        // Notify libscratchcpp about the click
        m_engine->clickTarget(scratchTarget());
    }
}

void RenderedTarget::mouseReleaseEvent(QMouseEvent *event)
{
    m_clicked = false;
    Q_ASSERT(m_mouseArea);
    Q_ASSERT(m_engine);

    // Stop dragging
    if (m_mouseArea->draggedSprite() == this)
        m_mouseArea->setDraggedSprite(nullptr);
    else if (m_engine && m_spriteModel && m_spriteModel->sprite()->draggable()) {
        // Notify libscratchcpp about the click
        m_engine->clickTarget(scratchTarget());
    }
}

void RenderedTarget::mouseMoveEvent(QMouseEvent *event)
{
    Q_ASSERT((m_spriteModel && m_spriteModel->sprite()) || m_stageModel);
    Q_ASSERT(m_mouseArea);

    // Start dragging
    if (m_clicked && !m_mouseArea->draggedSprite() && m_spriteModel && m_spriteModel->sprite()->draggable()) {
        Q_ASSERT(m_engine);
        Sprite *sprite = m_spriteModel->sprite();
        m_dragDeltaX = m_engine->mouseX() - sprite->x();
        m_dragDeltaY = m_engine->mouseY() - sprite->y();
        m_mouseArea->setDraggedSprite(this);

        // Move the sprite to the front layer
        m_engine->moveSpriteToFront(sprite);
    }
}

Texture RenderedTarget::texture() const
{
    return m_texture;
}

void RenderedTarget::updateHullPoints(QOpenGLFramebufferObject *fbo)
{
    if (m_stageModel)
        return; // hull points are useless for the stage

    Q_ASSERT(fbo);
    int width = fbo->width();
    int height = fbo->height();
    m_hullPoints.clear();
    m_hullPoints.reserve(width * height);

    // Blit multisampled FBO to a custom FBO
    QOpenGLFramebufferObject customFbo(fbo->size());
    glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, fbo->handle());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER_EXT, customFbo.handle());
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER_EXT, customFbo.handle());

    // Read pixels from framebuffer
    size_t size = width * height * 4;
    GLubyte *pixelData = new GLubyte[size];
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
    fbo->bind();

    // Flip vertically
    int rowSize = width * 4;
    GLubyte *tempRow = new GLubyte[rowSize];

    for (size_t i = 0; i < height / 2; ++i) {
        size_t topRowIndex = i * rowSize;
        size_t bottomRowIndex = (height - 1 - i) * rowSize;

        // Swap rows
        memcpy(tempRow, &pixelData[topRowIndex], rowSize);
        memcpy(&pixelData[topRowIndex], &pixelData[bottomRowIndex], rowSize);
        memcpy(&pixelData[bottomRowIndex], tempRow, rowSize);
    }

    delete[] tempRow;

    // Fill hull points vector
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 4; // RGBA channels

            // Check alpha channel
            if (pixelData[index + 3] > 0)
                m_hullPoints.push_back(QPointF(x, y));
        }
    }

    delete[] pixelData;
}

const std::vector<QPointF> &RenderedTarget::hullPoints() const
{
    return m_hullPoints;
}

bool RenderedTarget::contains(const QPointF &point) const
{
    if (m_stageModel)
        return true; // the stage contains any point within the scene

    if (!boundingRect().contains(point))
        return false;

    QPoint intPoint = point.toPoint();
    auto it = std::lower_bound(m_hullPoints.begin(), m_hullPoints.end(), intPoint, [](const QPointF &lhs, const QPointF &rhs) {
        return (lhs.y() < rhs.y()) || (lhs.y() == rhs.y() && lhs.x() < rhs.x());
    });

    if (it == m_hullPoints.end()) {
        // The point is beyond the last point in the convex hull
        return false;
    }

    // Check if the point is equal to the one found
    return *it == intPoint;
}

void RenderedTarget::calculatePos()
{
    if (!m_skin || !m_costume || !m_engine)
        return;

    if (isVisible() || m_stageModel) {
        double stageWidth = m_engine->stageWidth();
        double stageHeight = m_engine->stageHeight();
        setX(m_stageScale * (stageWidth / 2 + m_x - m_costume->rotationCenterX() * m_size / scale() / m_costume->bitmapResolution() * (m_mirrorHorizontally ? -1 : 1)));
        setY(m_stageScale * (stageHeight / 2 - m_y - m_costume->rotationCenterY() * m_size / scale() / m_costume->bitmapResolution()));
        qreal originX = m_costume->rotationCenterX() * m_stageScale * m_size / scale() / m_costume->bitmapResolution();
        qreal originY = m_costume->rotationCenterY() * m_stageScale * m_size / scale() / m_costume->bitmapResolution();
        setTransformOriginPoint(QPointF(originX, originY));

        // Qt ignores the transform origin point if it's (0, 0),
        // so set the transform origin to top left in this case.
        if (originX == 0 && originY == 0)
            setTransformOrigin(QQuickItem::TopLeft);
        else
            setTransformOrigin(QQuickItem::Center);
    }
}

void RenderedTarget::calculateRotation()
{
    if (isVisible()) {
        // Direction
        bool oldMirrorHorizontally = m_mirrorHorizontally;

        switch (m_rotationStyle) {
            case Sprite::RotationStyle::AllAround:
                setRotation(m_direction - 90);
                m_mirrorHorizontally = (false);

                break;

            case Sprite::RotationStyle::LeftRight: {
                setRotation(0);
                m_mirrorHorizontally = (m_direction < 0);

                break;
            }

            case Sprite::RotationStyle::DoNotRotate:
                setRotation(0);
                m_mirrorHorizontally = false;
                break;
        }

        if (m_mirrorHorizontally != oldMirrorHorizontally)
            emit mirrorHorizontallyChanged();
    }
}

void RenderedTarget::calculateSize()
{
    if (m_skin && m_costume) {
        Texture texture = m_skin->getTexture(m_size * m_stageScale);
        m_texture = texture;
        m_width = texture.width();
        m_height = texture.height();
        setScale(m_size * m_stageScale / m_skin->getTextureScale(texture) / m_costume->bitmapResolution());
    }
}

void RenderedTarget::handleSceneMouseMove(qreal x, qreal y)
{
    Q_ASSERT(m_mouseArea);

    if (m_mouseArea->draggedSprite() == this) {
        Q_ASSERT(m_spriteModel && m_spriteModel->sprite());
        Q_ASSERT(m_engine);
        Sprite *sprite = m_spriteModel->sprite();
        sprite->setX(x / m_stageScale - m_engine->stageWidth() / 2.0 - m_dragDeltaX);
        sprite->setY(-y / m_stageScale + m_engine->stageHeight() / 2.0 - m_dragDeltaY);
    }
}

bool RenderedTarget::mirrorHorizontally() const
{
    return m_mirrorHorizontally;
}
