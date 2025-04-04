﻿// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QQmlEngine>
#include <QFuture>
#include <scratchcpp/project.h>
#include <scratchcpp/iengine.h>

#include "stagemodel.h"

Q_MOC_INCLUDE("spritemodel.h");
Q_MOC_INCLUDE("monitormodel.h");

class QSurface;
class QOpenGLContext;

namespace scratchcpprender
{

class SpriteModel;
class MonitorModel;

class ProjectLoader : public QObject
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
        Q_PROPERTY(LoadStatus loadStatus READ loadStatus NOTIFY loadStatusChanged)
        Q_PROPERTY(bool running READ running NOTIFY runningChanged)
        Q_PROPERTY(int renderFps READ renderFps NOTIFY renderFpsChanged FINAL)
        Q_PROPERTY(libscratchcpp::IEngine *engine READ engine NOTIFY engineChanged)
        Q_PROPERTY(StageModel *stage READ stage NOTIFY stageChanged)
        Q_PROPERTY(QQmlListProperty<SpriteModel> sprites READ sprites NOTIFY spritesChanged)
        Q_PROPERTY(QQmlListProperty<SpriteModel> clones READ clones NOTIFY clonesChanged)
        Q_PROPERTY(QQmlListProperty<MonitorModel> monitors READ monitors NOTIFY monitorsChanged)
        Q_PROPERTY(QStringList unsupportedBlocks READ unsupportedBlocks NOTIFY unsupportedBlocksChanged)
        Q_PROPERTY(double fps READ fps WRITE setFps NOTIFY fpsChanged)
        Q_PROPERTY(bool turboMode READ turboMode WRITE setTurboMode NOTIFY turboModeChanged)
        Q_PROPERTY(unsigned int stageWidth READ stageWidth WRITE setStageWidth NOTIFY stageWidthChanged)
        Q_PROPERTY(unsigned int stageHeight READ stageHeight WRITE setStageHeight NOTIFY stageHeightChanged)
        Q_PROPERTY(int cloneLimit READ cloneLimit WRITE setCloneLimit NOTIFY cloneLimitChanged)
        Q_PROPERTY(bool spriteFencing READ spriteFencing WRITE setSpriteFencing NOTIFY spriteFencingChanged)
        Q_PROPERTY(bool mute READ mute WRITE setMute NOTIFY muteChanged)
        Q_PROPERTY(unsigned int downloadedAssets READ downloadedAssets NOTIFY downloadedAssetsChanged)
        Q_PROPERTY(unsigned int assetCount READ assetCount NOTIFY assetCountChanged)

    public:
        enum class LoadStatus
        {
            Idle,
            Loading,
            Loaded,
            Failed,
            Aborted
        };

        Q_ENUM(LoadStatus)

        explicit ProjectLoader(QObject *parent = nullptr);
        ~ProjectLoader();

        const QString &fileName() const;
        void setFileName(const QString &newFileName);

        LoadStatus loadStatus() const;
        Q_INVOKABLE void stopLoading();

        bool running() const;

        libscratchcpp::IEngine *engine() const;
        void setEngine(libscratchcpp::IEngine *engine);

        StageModel *stage();

        QQmlListProperty<SpriteModel> sprites();
        const QList<SpriteModel *> &spriteList() const;

        QQmlListProperty<SpriteModel> clones();
        const QList<SpriteModel *> &cloneList() const;

        QQmlListProperty<MonitorModel> monitors();
        const QList<MonitorModel *> &monitorList() const;

        const QStringList &unsupportedBlocks() const;

        Q_INVOKABLE void start();
        Q_INVOKABLE void stop();

        Q_INVOKABLE void answerQuestion(const QString &answer);

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

        bool mute() const;
        void setMute(bool newMute);

        unsigned int downloadedAssets() const;

        unsigned int assetCount() const;

        int renderFps() const;

    signals:
        void fileNameChanged();
        void loadStatusChanged();
        void loadingFinished();
        void runningChanged();
        void renderFpsChanged();
        void engineChanged();
        void stageChanged();
        void spritesChanged();
        void clonesChanged();
        void monitorsChanged();
        void unsupportedBlocksChanged();
        void fpsChanged();
        void turboModeChanged();
        void stageWidthChanged();
        void stageHeightChanged();
        void cloneLimitChanged();
        void spriteFencingChanged();
        void muteChanged();
        void downloadedAssetsChanged();
        void assetCountChanged();
        void cloneCreated(SpriteModel *model);
        void cloneDeleted(SpriteModel *model);
        void monitorAdded(MonitorModel *model);
        void monitorRemoved(MonitorModel *model);
        void questionAsked(QString question);
        void questionAborted();

    protected:
        void timerEvent(QTimerEvent *event) override;

    private:
        static void callLoad(ProjectLoader *loader);
        void clear();
        void load();
        void initTimer();
        void redraw();
        void addClone(SpriteModel *model);
        void deleteCloneObject(SpriteModel *model);
        void deleteClone(SpriteModel *model);
        void addMonitor(libscratchcpp::Monitor *monitor);
        void removeMonitor(libscratchcpp::Monitor *monitor, libscratchcpp::IMonitorHandler *iface);

        int m_timerId = -1;
        QString m_fileName;
        QFuture<void> m_loadThread;
        libscratchcpp::Project m_project;
        bool m_running = false;
        QElapsedTimer m_renderTimer;
        int m_renderFps = 0;
        int m_renderFpsCounter = 0;
        libscratchcpp::IEngine *m_engine = nullptr;
        libscratchcpp::IEngine *m_oldEngine = nullptr;
        QMutex m_engineMutex;
        LoadStatus m_loadStatus = LoadStatus::Idle;
        StageModel m_stage;
        QList<SpriteModel *> m_sprites;
        QList<SpriteModel *> m_clones;
        QList<SpriteModel *> m_emptySpriteList;
        QList<MonitorModel *> m_monitors;
        std::vector<libscratchcpp::Monitor *> m_unpositionedMonitors;
        QStringList m_unsupportedBlocks;
        double m_fps = 30;
        bool m_turboMode = false;
        unsigned int m_stageWidth = 480;
        unsigned int m_stageHeight = 360;
        int m_cloneLimit = 300;
        bool m_spriteFencing = true;
        bool m_mute = false;
        std::atomic<unsigned int> m_downloadedAssets = 0;
        std::atomic<unsigned int> m_assetCount = 0;
        std::atomic<bool> m_stopLoading = false;
        QOpenGLContext *m_glCtx = nullptr;
        QSurface *m_surface = nullptr;
};

} // namespace scratchcpprender
