// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/iengine.h>
#include <scratchcpp/costume.h>
#include <scratchcpp/rect.h>
#include <QtSvg/QSvgRenderer>
#include <qnanopainter.h>

#include "renderedtarget.h"
#include "targetpainter.h"
#include "stagemodel.h"
#include "spritemodel.h"
#include "scenemousearea.h"
#include "bitmapskin.h"
#include "svgskin.h"
#include "cputexturemanager.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

static const double SVG_SCALE_LIMIT = 0.1; // the maximum viewport dimensions are multiplied by this
static const double pi = std::acos(-1);    // TODO: Use std::numbers::pi in C++20

RenderedTarget::RenderedTarget(QQuickItem *parent) :
    IRenderedTarget(parent)
{
    setSmooth(false);
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
    m_convexHullDirty = true;
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

    setSmooth(m_costume->dataFormat() == "svg");

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
    m_costume = nullptr;
    m_costumesLoaded = false;

    if (!m_skinsInherited) {
        for (const auto &[costume, skin] : m_skins)
            delete skin;

        m_skins.clear();
    }

    m_skin = nullptr;
    m_texture = Texture();
    m_oldTexture = Texture();
    m_convexHullDirty = true;
    clearGraphicEffects();
    m_hullPoints.clear();

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
            // Inherit skins, texture mananger, convex hull points, etc. from the clone root
            RenderedTarget *target = dynamic_cast<RenderedTarget *>(cloneRoot->renderedTarget());
            Q_ASSERT(target);
            m_textureManager = target->m_textureManager;
            m_convexHullDirty = target->m_convexHullDirty;
            m_hullPoints = target->m_hullPoints;

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
    beforeRedraw();
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

Rect RenderedTarget::getBounds() const
{
    // https://github.com/scratchfoundation/scratch-render/blob/c3ede9c3d54769730c7b023021511e2aba167b1f/src/Rectangle.js#L33-L55
    if (!m_costume || !m_skin || !m_texture.isValid())
        return Rect(m_x, m_y, m_x, m_y);

    const double width = m_texture.width() * m_size / scale() / m_costume->bitmapResolution();
    const double height = m_texture.height() * m_size / scale() / m_costume->bitmapResolution();
    const double originX = m_stageScale * m_costume->rotationCenterX() * m_size / scale() / m_costume->bitmapResolution() - width / 2;
    const double originY = m_stageScale * -m_costume->rotationCenterY() * m_size / scale() / m_costume->bitmapResolution() + height / 2;
    const double rot = -rotation() * pi / 180;
    double left = std::numeric_limits<double>::infinity();
    double top = -std::numeric_limits<double>::infinity();
    double right = -std::numeric_limits<double>::infinity();
    double bottom = std::numeric_limits<double>::infinity();

    const std::vector<QPoint> &points = hullPoints();

    for (const QPointF &point : points) {
        QPointF transformed = transformPoint(point.x() - width / 2, height / 2 - point.y(), originX, originY, rot);
        const double x = transformed.x() * scale() / m_stageScale * (m_mirrorHorizontally ? -1 : 1);
        const double y = transformed.y() * scale() / m_stageScale;

        if (x < left)
            left = x;

        if (x > right)
            right = x;

        if (y > top)
            top = y;

        if (y < bottom)
            bottom = y;
    }

    return Rect(left + m_x, top + m_y, right + m_x, bottom + m_y);
}

QRectF RenderedTarget::getBoundsForBubble() const
{
    // https://github.com/scratchfoundation/scratch-render/blob/86dcb0151a04bc8c1ff39559e8531e7921102b56/src/Drawable.js#L536-L551
    Rect rect = getBounds();
    const int slice = 8; // px, how tall the top slice to measure should be

    if (rect.height() > slice)
        rect.setBottom(rect.top() - slice);

    Q_ASSERT(rect.height() <= 8 || std::abs(rect.bottom()) == std::numeric_limits<double>::infinity() || std::abs(rect.top()) == std::numeric_limits<double>::infinity());
    return QRectF(QPointF(rect.left(), rect.top()), QPointF(rect.right(), rect.bottom()));
}

Rect RenderedTarget::getFastBounds() const
{
    if (!m_costume || !m_skin || !m_texture.isValid())
        return Rect(m_x, m_y, m_x, m_y);

    const double scale = this->scale();
    const double width = this->width() / m_stageScale;
    const double height = this->height() / m_stageScale;
    const double originX = m_costume->rotationCenterX() * m_size / scale / m_costume->bitmapResolution() - width / 2;
    const double originY = -m_costume->rotationCenterY() * m_size / scale / m_costume->bitmapResolution() + height / 2;
    const double rot = -rotation() * pi / 180;

    QPointF topLeft = transformPoint(-width / 2, height / 2, originX, originY, rot);
    QPointF topRight = transformPoint(width / 2, height / 2, originX, originY, rot);
    QPointF bottomRight = transformPoint(width / 2, -height / 2, originX, originY, rot);
    QPointF bottomLeft = transformPoint(-width / 2, -height / 2, originX, originY, rot);

    if (m_mirrorHorizontally) {
        topLeft.setX(-topLeft.x());
        topRight.setX(-topRight.x());
        bottomRight.setX(-bottomRight.x());
        bottomLeft.setX(-bottomLeft.x());
    }
    const auto xList = { topLeft.x(), topRight.x(), bottomRight.x(), bottomLeft.x() };
    const auto yList = { topLeft.y(), topRight.y(), bottomRight.y(), bottomLeft.y() };
    const double minX = std::min(xList);
    const double maxX = std::max(xList);
    const double minY = std::min(yList);
    const double maxY = std::max(yList);

    return Rect(minX * scale + m_x, maxY * scale + m_y, maxX * scale + m_x, minY * scale + m_y);
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

const std::unordered_map<ShaderManager::Effect, double> &RenderedTarget::graphicEffects() const
{
    return m_graphicEffects;
}

void RenderedTarget::setGraphicEffect(ShaderManager::Effect effect, double value)
{
    bool changed = false;
    auto it = m_graphicEffects.find(effect);

    if (value == 0) {
        if (it != m_graphicEffects.cend()) {
            changed = true;
            m_graphicEffects.erase(effect);
        }
    } else {
        if (it != m_graphicEffects.cend())
            changed = it->second != value;
        else
            changed = true;

        m_graphicEffects[effect] = value;
    }

    if (changed)
        update();

    // TODO: Set m_convexHullDirty to true if the effect changes shape
}

void RenderedTarget::clearGraphicEffects()
{
    if (!m_graphicEffects.empty())
        update();

    // TODO: Set m_convexHullDirty to true if any of the previous effects changed shape
    m_graphicEffects.clear();
}

const std::vector<QPoint> &RenderedTarget::hullPoints() const
{
    if (convexHullPointsNeeded())
        const_cast<RenderedTarget *>(this)->updateHullPoints();

    return m_hullPoints;
}

bool RenderedTarget::contains(const QPointF &point) const
{
    if (m_stageModel)
        return true; // the stage contains any point within the scene

    if (!boundingRect().contains(point))
        return false;

    const std::vector<QPoint> &points = hullPoints();

    QPoint intPoint = point.toPoint();
    auto it = std::lower_bound(points.begin(), points.end(), intPoint, [](const QPointF &lhs, const QPointF &rhs) { return (lhs.y() < rhs.y()) || (lhs.y() == rhs.y() && lhs.x() < rhs.x()); });

    if (it == points.end()) {
        // The point is beyond the last point in the convex hull
        return false;
    }

    // Check if the point is equal to the one found
    return *it == intPoint;
}

bool RenderedTarget::containsScratchPoint(double x, double y) const
{
    if (!m_engine || !parentItem())
        return false;

    // contains() expects item coordinates, so translate the Scratch coordinates first
    double stageWidth = m_engine->stageWidth();
    double stageHeight = m_engine->stageHeight();
    x = m_stageScale * (x + stageWidth / 2);
    y = m_stageScale * (stageHeight / 2 - y);

    return contains(mapFromItem(parentItem(), QPointF(x, y)));
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
        GLuint oldTexture = m_texture.handle();
        bool wasValid = m_texture.isValid();
        m_texture = m_skin->getTexture(m_size * m_stageScale);
        m_width = m_texture.width();
        m_height = m_texture.height();
        setScale(m_size * m_stageScale / m_skin->getTextureScale(m_texture) / m_costume->bitmapResolution());

        if (wasValid && m_texture.handle() != oldTexture)
            m_convexHullDirty = true;
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

bool RenderedTarget::convexHullPointsNeeded() const
{
    return m_convexHullDirty || m_hullPoints.empty();
}

void RenderedTarget::updateHullPoints()
{
    m_convexHullDirty = false;

    if (!isVisible()) {
        m_hullPoints.clear();
        return;
    }

    m_hullPoints = textureManager()->getTextureConvexHullPoints(m_texture);
    // TODO: Apply graphic effects (#117)
}

QPointF RenderedTarget::transformPoint(double scratchX, double scratchY, double originX, double originY, double rot) const
{
    const double cosRot = std::cos(rot);
    const double sinRot = std::sin(rot);
    const double x = (scratchX - originX) * cosRot - (scratchY - originY) * sinRot;
    const double y = (scratchX - originX) * sinRot + (scratchY - originY) * cosRot;
    return QPointF(x, y);
}

CpuTextureManager *RenderedTarget::textureManager()
{
    if (!m_textureManager)
        m_textureManager = std::make_shared<CpuTextureManager>();

    return m_textureManager.get();
}

bool RenderedTarget::mirrorHorizontally() const
{
    return m_mirrorHorizontally;
}
