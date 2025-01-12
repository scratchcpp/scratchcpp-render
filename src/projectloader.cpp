// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/iengine.h>
#include <scratchcpp/value.h>
#include <scratchcpp/monitor.h>
#include <scratchcpp/scratchconfiguration.h>
#include <QtConcurrent/QtConcurrent>
#include <QApplication>

#include "projectloader.h"
#include "spritemodel.h"
#include "valuemonitormodel.h"
#include "listmonitormodel.h"
#include "renderedtarget.h"
#include "blocks/penblocks.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

ProjectLoader::ProjectLoader(QObject *parent) :
    QObject(parent)
{
    m_project.downloadProgressChanged().connect([this](unsigned int finished, unsigned int all) {
        if (finished != m_downloadedAssets) {
            m_downloadedAssets = finished;
            emit downloadedAssetsChanged();
        }

        if (all != m_assetCount) {
            m_assetCount = all;
            emit assetCountChanged();
        }
    });

    connect(qApp, &QCoreApplication::aboutToQuit, this, &ProjectLoader::clear);

    initTimer();
    m_renderTimer.start();

#ifndef USE_LLVM
    // Register pen blocks
    ScratchConfiguration::registerExtension(std::make_shared<PenBlocks>());
#endif
}

ProjectLoader::~ProjectLoader()
{
    stopLoading();

    for (SpriteModel *sprite : m_sprites)
        sprite->deleteLater();
}

const QString &ProjectLoader::fileName() const
{
    return m_fileName;
}

void ProjectLoader::setFileName(const QString &newFileName)
{
    if (m_loadThread.isRunning()) {
        stopLoading();
        m_loadThread.waitForFinished();
        QCoreApplication::processEvents();
    }

    if (newFileName.isEmpty())
        return;

    m_fileName = newFileName;

    clear();

    m_project.setFileName(m_fileName.toStdString());
    m_loadStatus = LoadStatus::Loading;

    // TODO: Do not set these to 0 after libscratchcpp starts doing it itself
    m_downloadedAssets = 0;
    m_assetCount = 0;
    emit downloadedAssetsChanged();
    emit assetCountChanged();

    emit loadStatusChanged();
    emit fileNameChanged();

    m_stopLoading = false;
    m_loadThread = QtConcurrent::run(&callLoad, this);
}

ProjectLoader::LoadStatus ProjectLoader::loadStatus() const
{
    if (m_loadThread.isRunning())
        return LoadStatus::Loading;

    return m_loadStatus;
}

void ProjectLoader::stopLoading()
{
    if (m_loadThread.isRunning()) {
        m_project.stopLoading();
        m_stopLoading = true;
        m_loadThread.waitForFinished();
    }
}

bool ProjectLoader::running() const
{
    return m_running;
}

int ProjectLoader::renderFps() const
{
    return m_renderFps;
}

IEngine *ProjectLoader::engine() const
{
    if (m_loadThread.isRunning())
        return nullptr;

    return m_engine;
}

// NOTE: This should be only used for testing
void ProjectLoader::setEngine(libscratchcpp::IEngine *engine)
{
    m_engine = engine;
}

StageModel *ProjectLoader::stage()
{
    if (m_loadThread.isRunning())
        return nullptr;

    return &m_stage;
}

QQmlListProperty<SpriteModel> ProjectLoader::sprites()
{
    if (m_loadThread.isRunning()) {
        m_emptySpriteList.clear();
        return QQmlListProperty<SpriteModel>(this, &m_emptySpriteList);
    }

    return QQmlListProperty<SpriteModel>(this, &m_sprites);
}

const QList<SpriteModel *> &ProjectLoader::spriteList() const
{
    return m_sprites;
}

QQmlListProperty<SpriteModel> ProjectLoader::clones()
{
    return QQmlListProperty<SpriteModel>(this, &m_clones);
}

const QList<SpriteModel *> &ProjectLoader::cloneList() const
{
    return m_clones;
}

QQmlListProperty<MonitorModel> ProjectLoader::monitors()
{
    return QQmlListProperty<MonitorModel>(this, &m_monitors);
}

const QList<MonitorModel *> &ProjectLoader::monitorList() const
{
    return m_monitors;
}

const QStringList &ProjectLoader::unsupportedBlocks() const
{
    return m_unsupportedBlocks;
}

void ProjectLoader::start()
{
    if (m_loadThread.isRunning())
        return;

    if (m_loadStatus == LoadStatus::Loaded) {
        Q_ASSERT(m_engine);
        m_engine->start();
    }
}

void ProjectLoader::stop()
{
    if (m_loadThread.isRunning())
        return;

    if (m_loadStatus == LoadStatus::Loaded) {
        Q_ASSERT(m_engine);
        m_engine->stop();
    }
}

void ProjectLoader::answerQuestion(const QString &answer)
{
    if (m_engine)
        m_engine->questionAnswered()(answer.toStdString());
}

void ProjectLoader::timerEvent(QTimerEvent *event)
{
    if (m_loadThread.isRunning())
        return;

    if (m_engine) {
        for (Monitor *monitor : m_unpositionedMonitors)
            monitor->autoPosition(m_engine->monitors());

        m_unpositionedMonitors.clear();

        m_engine->step();

        if (m_running != m_engine->isRunning()) {
            m_running = !m_running;
            emit runningChanged();
        }

        // FPS counter
        if (m_renderTimer.elapsed() >= 1000) {
            m_renderFps = m_renderFpsCounter;
            m_renderFpsCounter = 0;
            emit renderFpsChanged();
            m_renderTimer.restart();
        } else
            m_renderFpsCounter++;
    }

    event->accept();
}

void ProjectLoader::callLoad(ProjectLoader *loader)
{
    loader->load();
}

void ProjectLoader::clear()
{
    // Stop the project
    if (m_engine)
        m_engine->stop();

    // Reset stage model
    m_stage.init(nullptr);

    if (m_stage.renderedTarget())
        m_stage.renderedTarget()->update();

    // Delete old sprites
    for (SpriteModel *sprite : m_sprites)
        sprite->deleteLater();

    m_sprites.clear();
    emit spritesChanged();

    // Delete old clones
    for (SpriteModel *clone : m_clones)
        deleteCloneObject(clone);

    m_clones.clear();
    emit clonesChanged();

    // Delete old monitors
    for (MonitorModel *monitor : m_monitors) {
        emit monitorRemoved(monitor);
        monitor->deleteLater();
    }

    m_monitors.clear();
    emit monitorsChanged();

    // Clear the engine
    if (m_engine)
        m_engine->clear();

    m_oldEngine = m_engine;
    m_engine = nullptr;
    emit engineChanged();
}

void ProjectLoader::load()
{
    m_unpositionedMonitors.clear();
    m_loadStatus = m_project.load() ? LoadStatus::Loaded : LoadStatus::Failed;
    m_engineMutex.lock();
    m_engine = m_project.engine().get();

    if (!m_engine || m_stopLoading) {
        m_engineMutex.unlock();

        if (m_stopLoading)
            m_loadStatus = LoadStatus::Aborted;

        emit fileNameChanged();
        emit loadStatusChanged();
        emit loadingFinished();
        emit engineChanged();
        emit spritesChanged();
        return;
    }

    m_engine->setFps(m_fps);
    m_engine->setTurboModeEnabled(m_turboMode);
    m_engine->setStageWidth(m_stageWidth);
    m_engine->setStageHeight(m_stageHeight);
    m_engine->setCloneLimit(m_cloneLimit);
    m_engine->setSpriteFencingEnabled(m_spriteFencing);
    m_engine->setGlobalVolume(m_mute ? 0 : 100);

    if (m_engine != m_oldEngine) {
        m_engine->aboutToRender().connect(&ProjectLoader::redraw, this);
        m_engine->monitorAdded().connect(&ProjectLoader::addMonitor, this);
        m_engine->monitorRemoved().connect(&ProjectLoader::removeMonitor, this);

        m_engine->questionAsked().connect([this](const std::string &question) { emit questionAsked(QString::fromStdString(question)); });
        m_engine->questionAborted().connect([this]() { emit questionAborted(); });
    }

    // Load targets
    const auto &targets = m_engine->targets();

    for (auto target : targets) {
        if (target->isStage())
            dynamic_cast<Stage *>(target.get())->setInterface(&m_stage);
        else {
            SpriteModel *sprite = new SpriteModel;
            sprite->moveToThread(qApp->thread());
            dynamic_cast<Sprite *>(target.get())->setInterface(sprite);
            connect(sprite, &SpriteModel::cloned, this, &ProjectLoader::addClone);
            m_sprites.push_back(sprite);
        }
    }

    // Load monitors
    const auto &monitors = m_engine->monitors();

    for (auto monitor : monitors)
        addMonitor(monitor.get());

    if (m_stopLoading) {
        m_engineMutex.unlock();
        m_loadStatus = LoadStatus::Aborted;
        emit fileNameChanged();
        emit loadStatusChanged();
        emit loadingFinished();
        emit engineChanged();
        emit spritesChanged();
        return;
    }

    // Get unsupported blocks
    const auto &unsupportedBlocks = m_engine->unsupportedBlocks();
    m_unsupportedBlocks.clear();

    for (const std::string &opcode : unsupportedBlocks)
        m_unsupportedBlocks.push_back(QString::fromStdString(opcode));

    emit unsupportedBlocksChanged();

    m_engineMutex.unlock();

    emit loadStatusChanged();
    emit loadingFinished();
    emit engineChanged();
    emit stageChanged();
    emit spritesChanged();
}

void ProjectLoader::initTimer()
{
    m_timerId = startTimer(1000 / m_fps);
}

void ProjectLoader::redraw()
{
    if (m_loadThread.isRunning())
        return;

    auto stage = m_stage.renderedTarget();

    if (stage)
        stage->beforeRedraw();

    for (auto sprite : m_sprites) {
        auto renderedTarget = sprite->renderedTarget();

        if (renderedTarget)
            renderedTarget->beforeRedraw();
    }

    for (auto sprite : m_clones) {
        auto renderedTarget = sprite->renderedTarget();

        if (renderedTarget)
            renderedTarget->beforeRedraw();
    }

    m_engine->updateMonitors();
}

void ProjectLoader::addClone(SpriteModel *model)
{
    connect(model, &SpriteModel::cloneDeleted, this, &ProjectLoader::deleteClone);
    connect(model, &SpriteModel::cloned, this, &ProjectLoader::addClone);
    m_clones.push_back(model);
    emit cloneCreated(model);
    emit clonesChanged();
}

void ProjectLoader::deleteCloneObject(SpriteModel *model)
{
    emit cloneDeleted(model);
    Q_ASSERT(model->renderedTarget());
    model->renderedTarget()->deinitClone();
    model->deleteLater();
}

void ProjectLoader::deleteClone(SpriteModel *model)
{
    m_clones.removeAll(model);
    deleteCloneObject(model);
    emit clonesChanged();
}

void ProjectLoader::addMonitor(Monitor *monitor)
{
    auto extension = monitor->extension();

    if (!extension)
        return;

    MonitorModel *model = nullptr;

    switch (monitor->mode()) {
        case Monitor::Mode::List:
            model = new ListMonitorModel(extension);
            break;

        default:
            model = new ValueMonitorModel(extension);
            break;
    }

    if (!model)
        return;

    if (monitor->needsAutoPosition()) {
        if (std::find(m_unpositionedMonitors.begin(), m_unpositionedMonitors.end(), monitor) == m_unpositionedMonitors.end())
            m_unpositionedMonitors.push_back(monitor);
    }

    model->moveToThread(qApp->thread());
    model->setParent(this);
    monitor->setInterface(model);
    m_monitors.push_back(model);
    emit monitorAdded(model);
    emit monitorsChanged();
}

void ProjectLoader::removeMonitor(Monitor *monitor, IMonitorHandler *iface)
{
    MonitorModel *model = dynamic_cast<MonitorModel *>(iface);

    if (!model)
        return;

    m_monitors.removeAll(model);
    emit monitorRemoved(model);
    emit monitorsChanged();
}

double ProjectLoader::fps() const
{
    return m_fps;
}

void ProjectLoader::setFps(double newFps)
{
    if (qFuzzyCompare(m_fps, newFps))
        return;

    m_engineMutex.lock();

    if (m_engine) {
        m_engine->setFps(newFps);
        m_fps = m_engine->fps();
    } else
        m_fps = newFps;

    killTimer(m_timerId);
    initTimer();

    m_engineMutex.unlock();
    emit fpsChanged();
}

bool ProjectLoader::turboMode() const
{
    return m_turboMode;
}

void ProjectLoader::setTurboMode(bool newTurboMode)
{
    if (m_turboMode == newTurboMode)
        return;

    m_turboMode = newTurboMode;
    m_engineMutex.lock();

    if (m_engine)
        m_engine->setTurboModeEnabled(m_turboMode);

    m_engineMutex.unlock();
    emit turboModeChanged();
}

unsigned int ProjectLoader::stageWidth() const
{
    return m_stageWidth;
}

void ProjectLoader::setStageWidth(unsigned int newStageWidth)
{
    if (m_stageWidth == newStageWidth)
        return;

    m_stageWidth = newStageWidth;
    m_engineMutex.lock();

    if (m_engine)
        m_engine->setStageWidth(m_stageWidth);

    m_engineMutex.unlock();
    emit stageWidthChanged();
}

unsigned int ProjectLoader::stageHeight() const
{
    return m_stageHeight;
}

void ProjectLoader::setStageHeight(unsigned int newStageHeight)
{
    if (m_stageHeight == newStageHeight)
        return;

    m_stageHeight = newStageHeight;
    m_engineMutex.lock();

    if (m_engine)
        m_engine->setStageHeight(m_stageHeight);

    m_engineMutex.unlock();
    emit stageHeightChanged();
}

int ProjectLoader::cloneLimit() const
{
    return m_cloneLimit;
}

void ProjectLoader::setCloneLimit(int newCloneLimit)
{
    if (m_cloneLimit == newCloneLimit)
        return;

    m_engineMutex.lock();

    if (m_engine) {
        m_engine->setCloneLimit(newCloneLimit);
        m_cloneLimit = m_engine->cloneLimit();
    } else
        m_cloneLimit = newCloneLimit;

    m_engineMutex.unlock();
    emit cloneLimitChanged();
}

bool ProjectLoader::spriteFencing() const
{
    return m_spriteFencing;
}

void ProjectLoader::setSpriteFencing(bool newSpriteFencing)
{
    if (m_spriteFencing == newSpriteFencing)
        return;

    m_spriteFencing = newSpriteFencing;
    m_engineMutex.lock();

    if (m_engine)
        m_engine->setSpriteFencingEnabled(m_spriteFencing);

    m_engineMutex.unlock();
    emit spriteFencingChanged();
}

bool ProjectLoader::mute() const
{
    return m_mute;
}

void ProjectLoader::setMute(bool newMute)
{
    if (m_mute == newMute)
        return;

    m_mute = newMute;
    m_engineMutex.lock();

    if (m_engine)
        m_engine->setGlobalVolume(newMute ? 0 : 100);

    m_engineMutex.unlock();
    emit muteChanged();
}

unsigned int ProjectLoader::downloadedAssets() const
{
    return m_downloadedAssets;
}

unsigned int ProjectLoader::assetCount() const
{
    return m_assetCount;
}
