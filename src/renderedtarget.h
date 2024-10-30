// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <qnanoquickitem.h>
#include <QBuffer>
#include <QMutex>
#include <QtSvg/QSvgRenderer>
#include <QImage>

#include "irenderedtarget.h"
#include "texture.h"

Q_MOC_INCLUDE("stagemodel.h");
Q_MOC_INCLUDE("spritemodel.h");
Q_MOC_INCLUDE("scenemousearea.h");

namespace scratchcpprender
{

class Skin;
class CpuTextureManager;
class IPenLayer;

class RenderedTarget : public IRenderedTarget
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(libscratchcpp::IEngine *engine READ engine WRITE setEngine NOTIFY engineChanged)
        Q_PROPERTY(StageModel *stageModel READ stageModel WRITE setStageModel NOTIFY stageModelChanged)
        Q_PROPERTY(SpriteModel *spriteModel READ spriteModel WRITE setSpriteModel NOTIFY spriteModelChanged)
        Q_PROPERTY(bool mirrorHorizontally READ mirrorHorizontally NOTIFY mirrorHorizontallyChanged)
        Q_PROPERTY(SceneMouseArea *mouseArea READ mouseArea WRITE setMouseArea NOTIFY mouseAreaChanged)
        Q_PROPERTY(double stageScale READ stageScale WRITE setStageScale NOTIFY stageScaleChanged)

    public:
        RenderedTarget(QQuickItem *parent = nullptr);
        ~RenderedTarget();

        void updateVisibility(bool visible) override;
        void updateX(double x) override;
        void updateY(double y) override;
        void updateSize(double size) override;
        void updateDirection(double direction) override;
        void updateRotationStyle(libscratchcpp::Sprite::RotationStyle style) override;
        void updateLayerOrder(int layerOrder) override;
        void updateCostume(libscratchcpp::Costume *costume) override;

        bool costumesLoaded() const override;
        void loadCostumes() override;

        void beforeRedraw() override;

        void deinitClone() override;

        libscratchcpp::IEngine *engine() const override;
        void setEngine(libscratchcpp::IEngine *newEngine) override;

        StageModel *stageModel() const override;
        void setStageModel(StageModel *newStageModel) override;

        SpriteModel *spriteModel() const override;
        void setSpriteModel(SpriteModel *newSpriteModel) override;

        libscratchcpp::Target *scratchTarget() const override;

        SceneMouseArea *mouseArea() const override;
        void setMouseArea(SceneMouseArea *newMouseArea) override;

        double stageScale() const override;
        void setStageScale(double newStageScale) override;

        qreal width() const override;
        void setWidth(qreal width) override;

        qreal height() const override;
        void setHeight(qreal height) override;

        libscratchcpp::Rect getBounds() const override;
        Q_INVOKABLE QRectF getQmlBounds() const;
        Q_INVOKABLE QRectF getBoundsForBubble() const override;
        libscratchcpp::Rect getFastBounds() const override;

        QPointF mapFromScene(const QPointF &point) const override;

        bool mirrorHorizontally() const override;

        Texture texture() const override;
        const Texture &cpuTexture() const override;
        int costumeWidth() const override;
        int costumeHeight() const override;

        const std::unordered_map<ShaderManager::Effect, double> &graphicEffects() const override;
        void setGraphicEffect(ShaderManager::Effect effect, double value) override;
        void clearGraphicEffects() override;

        const std::vector<QPoint> &hullPoints() const override;

        Q_INVOKABLE bool contains(const QPointF &point) const override;
        bool containsScratchPoint(double x, double y) const override;
        QRgb colorAtScratchPoint(double x, double y) const override;

        bool touchingClones(const std::vector<libscratchcpp::Sprite *> &) const override;
        bool touchingColor(const libscratchcpp::Value &color) const override;
        bool touchingColor(const libscratchcpp::Value &color, const libscratchcpp::Value &mask) const override;

    signals:
        void engineChanged();
        void stageModelChanged();
        void spriteModelChanged();
        void mouseAreaChanged();
        void mirrorHorizontallyChanged();
        void stageScaleChanged();

    protected:
        QNanoQuickItemPainter *createItemPainter() const override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;

    private:
        void calculatePos();
        void calculateRotation();
        void calculateSize();
        void handleSceneMouseMove(qreal x, qreal y);
        bool convexHullPointsNeeded() const;
        void updateHullPoints();
        const std::vector<QPointF> &transformedHullPoints() const;
        bool containsLocalPoint(const QPointF &point) const;
        QPointF transformPoint(double scratchX, double scratchY, double originX, double originY, double rot) const;
        QPointF transformPoint(double scratchX, double scratchY, double originX, double originY, double sinRot, double cosRot) const;
        QPointF mapFromStageWithOriginPoint(const QPointF &scenePoint) const;
        QPointF mapFromScratchToLocal(const QPointF &point) const;
        CpuTextureManager *textureManager() const;
        bool touchingColor(const libscratchcpp::Value &color, bool hasMask, const libscratchcpp::Value &mask) const;
        QRectF touchingBounds() const;
        QRectF candidatesBounds(const QRectF &targetRect, const std::vector<libscratchcpp::Target *> &candidates, std::vector<IRenderedTarget *> &dst) const;
        QRectF candidatesBounds(const QRectF &targetRect, const std::vector<libscratchcpp::Sprite *> &candidates, std::vector<IRenderedTarget *> &dst) const;
        static QRectF candidateIntersection(const QRectF &targetRect, IRenderedTarget *target);
        static QRectF rectIntersection(const QRectF &targetRect, const libscratchcpp::Rect &candidateRect);
        static void clampRect(libscratchcpp::Rect &rect, double left, double right, double bottom, double top);
        static QRgb convertColor(const libscratchcpp::Value &color);
        static bool colorMatches(QRgb a, QRgb b);
        static bool maskMatches(QRgb a, QRgb b);
        QRgb sampleColor3b(double x, double y, const std::vector<IRenderedTarget *> &targets) const;

        libscratchcpp::IEngine *m_engine = nullptr;
        libscratchcpp::Costume *m_costume = nullptr;
        StageModel *m_stageModel = nullptr;
        SpriteModel *m_spriteModel = nullptr;
        SceneMouseArea *m_mouseArea = nullptr;
        IPenLayer *m_penLayer = nullptr;
        bool m_costumesLoaded = false;
        std::unordered_map<libscratchcpp::Costume *, Skin *> m_skins;
        bool m_skinsInherited = false;
        Skin *m_skin = nullptr;
        Texture m_texture;
        Texture m_oldTexture;
        Texture m_cpuTexture;                                        // without stage scale
        mutable std::shared_ptr<CpuTextureManager> m_textureManager; // NOTE: Use textureManager()!
        std::unique_ptr<QOpenGLFunctions> m_glF;
        mutable std::unordered_map<ShaderManager::Effect, double> m_graphicEffects;
        mutable ShaderManager::Effect m_graphicEffectMask = ShaderManager::Effect::NoEffect;
        double m_size = 1;
        double m_x = 0;
        double m_y = 0;
        double m_width = 1;
        double m_height = 1;
        double m_direction = 90;
        libscratchcpp::Sprite::RotationStyle m_rotationStyle = libscratchcpp::Sprite::RotationStyle::AllAround;
        bool m_mirrorHorizontally = false;
        double m_stageScale = 1;
        qreal m_maximumWidth = std::numeric_limits<double>::infinity();
        qreal m_maximumHeight = std::numeric_limits<double>::infinity();
        bool m_convexHullDirty = true;
        std::vector<QPoint> m_hullPoints;
        mutable bool m_transformedHullDirty = true;
        mutable std::vector<QPointF> m_transformedHullPoints; // NOTE: Use transformedHullPoints();
        bool m_clicked = false;                               // left mouse button only!
        double m_dragX = 0;
        double m_dragY = 0;
        double m_dragDeltaX = 0;
        double m_dragDeltaY = 0;
};

} // namespace scratchcpprender
