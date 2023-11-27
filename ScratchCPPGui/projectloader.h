// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QQmlEngine>
#include <QFuture>
#include <scratchcpp/project.h>
#include <scratchcpp/iengine.h>

#include "stagemodel.h"

namespace scratchcppgui
{

class SpriteModel;

class ProjectLoader : public QObject
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
        Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)
        Q_PROPERTY(libscratchcpp::IEngine *engine READ engine NOTIFY engineChanged)
        Q_PROPERTY(StageModel *stage READ stage NOTIFY stageChanged)
        Q_PROPERTY(QQmlListProperty<SpriteModel> sprites READ sprites NOTIFY spritesChanged)
        Q_PROPERTY(double fps READ fps WRITE setFps NOTIFY fpsChanged)
        Q_PROPERTY(bool turboMode READ turboMode WRITE setTurboMode NOTIFY turboModeChanged)
        Q_PROPERTY(unsigned int stageWidth READ stageWidth WRITE setStageWidth NOTIFY stageWidthChanged)
        Q_PROPERTY(unsigned int stageHeight READ stageHeight WRITE setStageHeight NOTIFY stageHeightChanged)
        Q_PROPERTY(int cloneLimit READ cloneLimit WRITE setCloneLimit NOTIFY cloneLimitChanged)
        Q_PROPERTY(bool spriteFencing READ spriteFencing WRITE setSpriteFencing NOTIFY spriteFencingChanged)

    public:
        explicit ProjectLoader(QObject *parent = nullptr);
        ~ProjectLoader();

        const QString &fileName() const;
        void setFileName(const QString &newFileName);

        bool loaded() const;

        libscratchcpp::IEngine *engine() const;

        StageModel *stage();

        QQmlListProperty<SpriteModel> sprites();

        Q_INVOKABLE void start();
        Q_INVOKABLE void stop();

        unsigned int frameFinished() const;
        void setFrameFinished(unsigned int newFrameFinished);

        double fps() const;
        void setFps(double newFps);

        bool turboMode() const;
        void setTurboMode(bool newTurboMode);

        unsigned int stageWidth() const;
        void setStageWidth(unsigned int newStageWidth);

        unsigned int stageHeight() const;
        void setStageHeight(unsigned int newStageHeight);

        int cloneLimit() const;
        void setCloneLimit(int newCloneLimit);

        bool spriteFencing() const;
        void setSpriteFencing(bool newSpriteFencing);

    signals:
        void fileNameChanged();
        void loadedChanged();
        void engineChanged();
        void stageChanged();
        void spritesChanged();
        void fpsChanged();
        void turboModeChanged();
        void stageWidthChanged();
        void stageHeightChanged();
        void cloneLimitChanged();
        void spriteFencingChanged();

    protected:
        void timerEvent(QTimerEvent *event) override;

    private:
        void initTimer();
        void emitTick();

        int m_timerId = -1;
        QString m_fileName;
        libscratchcpp::Project m_project;
        libscratchcpp::IEngine *m_engine = nullptr;
        bool m_loaded = false;
        StageModel m_stage;
        QList<SpriteModel *> m_sprites;
        QFuture<void> m_eventLoop;
        double m_fps = 30;
        bool m_turboMode = false;
        unsigned int m_stageWidth;
        unsigned int m_stageHeight;
        int m_cloneLimit;
        bool m_spriteFencing;
};

} // namespace scratchcppgui
