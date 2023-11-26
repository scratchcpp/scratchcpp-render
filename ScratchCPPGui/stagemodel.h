// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QObject>
#include <scratchcpp/istagehandler.h>

Q_MOC_INCLUDE("renderedtarget.h");

namespace scratchcppgui
{

class RenderedTarget;

class StageModel
    : public QObject
    , public libscratchcpp::IStageHandler
{
        Q_OBJECT
        Q_PROPERTY(RenderedTarget *renderedTarget READ renderedTarget WRITE setRenderedTarget NOTIFY renderedTargetChanged)

    public:
        explicit StageModel(QObject *parent = nullptr);

        void init(libscratchcpp::Stage *stage) override;

        void onCostumeChanged(libscratchcpp::Costume *costume) override;

        void onTempoChanged(int tempo) override;
        void onVideoStateChanged(libscratchcpp::Stage::VideoState videoState) override;
        void onVideoTransparencyChanged(int videoTransparency) override;

        libscratchcpp::Stage *stage() const;

        RenderedTarget *renderedTarget() const;
        void setRenderedTarget(RenderedTarget *newRenderedTarget);

    signals:
        void renderedTargetChanged();

    private:
        libscratchcpp::Stage *m_stage = nullptr;
        RenderedTarget *m_renderedTarget = nullptr;
};

} // namespace scratchcppgui