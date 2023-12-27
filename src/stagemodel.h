// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QObject>
#include <scratchcpp/istagehandler.h>

Q_MOC_INCLUDE("renderedtarget.h");

namespace scratchcpprender
{

class IRenderedTarget;

class StageModel
    : public QObject
    , public libscratchcpp::IStageHandler
{
        Q_OBJECT
        Q_PROPERTY(IRenderedTarget *renderedTarget READ renderedTarget WRITE setRenderedTarget NOTIFY renderedTargetChanged)

    public:
        explicit StageModel(QObject *parent = nullptr);

        void init(libscratchcpp::Stage *stage) override;

        void onCostumeChanged(libscratchcpp::Costume *costume) override;

        void onTempoChanged(int tempo) override;
        void onVideoStateChanged(libscratchcpp::Stage::VideoState videoState) override;
        void onVideoTransparencyChanged(int videoTransparency) override;

        void onGraphicsEffectChanged(libscratchcpp::IGraphicsEffect *effect, double value) override;
        void onGraphicsEffectsCleared() override;

        libscratchcpp::Stage *stage() const;

        IRenderedTarget *renderedTarget() const;
        void setRenderedTarget(IRenderedTarget *newRenderedTarget);

    signals:
        void renderedTargetChanged();

    private:
        libscratchcpp::Stage *m_stage = nullptr;
        IRenderedTarget *m_renderedTarget = nullptr;
};

} // namespace scratchcpprender
