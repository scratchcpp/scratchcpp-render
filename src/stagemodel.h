// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <scratchcpp/istagehandler.h>

#include "targetmodel.h"

namespace scratchcpprender
{

class StageModel
    : public TargetModel
    , public libscratchcpp::IStageHandler
{
        Q_OBJECT

    public:
        explicit StageModel(QObject *parent = nullptr);

        void init(libscratchcpp::Stage *stage) override;

        void onCostumeChanged(libscratchcpp::Costume *costume) override;

        void onTempoChanged(int tempo) override;
        void onVideoStateChanged(libscratchcpp::Stage::VideoState videoState) override;
        void onVideoTransparencyChanged(int videoTransparency) override;

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

        libscratchcpp::Stage *stage() const;

        int bubbleLayer() const override;

        Q_INVOKABLE void loadCostume() override;

    protected:
        void drawPenPoint(IPenLayer *penLayer, const PenAttributes &penAttributes) override;

    private:
        libscratchcpp::Stage *m_stage = nullptr;
};

} // namespace scratchcpprender
