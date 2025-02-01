// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <scratchcpp/ispritehandler.h>

#include "targetmodel.h"

namespace scratchcpprender
{

class SpriteModel
    : public TargetModel
    , public libscratchcpp::ISpriteHandler
{
        Q_OBJECT
        QML_ELEMENT

    public:
        SpriteModel(QObject *parent = nullptr);

        void init(libscratchcpp::Sprite *sprite) override;
        void deinitClone() override;

        void onCloned(libscratchcpp::Sprite *clone) override;

        void onCostumeChanged(libscratchcpp::Costume *costume) override;

        void onVisibleChanged(bool visible) override;
        void onXChanged(double x) override;
        void onYChanged(double y) override;
        void onMoved(double oldX, double oldY, double newX, double newY) override;
        void onSizeChanged(double size) override;
        void onDirectionChanged(double direction) override;
        void onRotationStyleChanged(libscratchcpp::Sprite::RotationStyle rotationStyle) override;
        void onLayerOrderChanged(int layerOrder) override;

        void onGraphicsEffectChanged(libscratchcpp::IGraphicsEffect *effect, double value) override;
        void onGraphicsEffectsCleared() override;

        int costumeWidth() const override;
        int costumeHeight() const override;

        libscratchcpp::Rect boundingRect() const override;
        libscratchcpp::Rect fastBoundingRect() const override;

        bool touchingClones(const std::vector<libscratchcpp::Sprite *> &clones) const override;
        bool touchingPoint(double x, double y) const override;
        bool touchingColor(libscratchcpp::Rgb color) const override;
        bool touchingColor(libscratchcpp::Rgb color, libscratchcpp::Rgb mask) const override;

        libscratchcpp::Sprite *sprite() const;

        int bubbleLayer() const override;

        SpriteModel *cloneRoot() const;

        Q_INVOKABLE void loadCostume() override;

    signals:
        void cloned(SpriteModel *cloneModel);
        void cloneDeleted(SpriteModel *clone);

    protected:
        void drawPenPoint(IPenLayer *penLayer, const PenAttributes &penAttributes) override;
        void drawPenLine(IPenLayer *penLayer, const PenAttributes &penAttributes, double x0, double y0, double x1, double y1) override;

    private:
        libscratchcpp::Sprite *m_sprite = nullptr;
        SpriteModel *m_cloneRoot = nullptr;
};

} // namespace scratchcpprender
