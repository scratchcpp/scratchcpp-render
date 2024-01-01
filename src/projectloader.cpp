// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/iengine.h>
#include <scratchcpp/value.h>
#include <QtConcurrent/QtConcurrent>
#include <QApplication>

#include "projectloader.h"
#include "spritemodel.h"
#include "renderedtarget.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

ProjectLoader::ProjectLoader(QObject *parent) :
    QObject(parent)
{
    m_project.setDownloadProgressCallback([this](unsigned int finished, unsigned int all) {
        if (finished != m_downloadedAssets) {
            m_downloadedAssets = finished;
            emit downloadedAssetsChanged();
        }

        if (all != m_assetCount) {
            m_assetCount = all;
            emit assetCountChanged();
        }
    });

    initTimer();
}

ProjectLoader::~ProjectLoader()
{
    m_stopLoading = true;

    if (m_loadThread.isRunning())
        m_loadThread.waitForFinished();

    for (SpriteModel *sprite : m_sprites)
        sprite->deleteLater();
}

const QString &ProjectLoader::fileName() const
{
    return m_fileName;
}

void ProjectLoader::setFileName(const QString &newFileName)
{
    if (m_loadThread.isRunning())
        m_loadThread.waitForFinished();

    if (m_fileName == newFileName)
        return;

    m_fileName = newFileName;

    m_project.setScratchVersion(ScratchVersion::Scratch3);
    m_project.setFileName(m_fileName.toStdString());
    m_loadStatus = false;

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

bool ProjectLoader::loadStatus() const
{
    if (m_loadThread.isRunning())
        return false;

    return m_loadStatus;
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
        m_loadThread.waitForFinished();

    return &m_stage;
}

QQmlListProperty<SpriteModel> ProjectLoader::sprites()
{
    if (m_loadThread.isRunning())
        m_loadThread.waitForFinished();

    return QQmlListProperty<SpriteModel>(this, &m_sprites);
}

const QList<SpriteModel *> &ProjectLoader::spriteList() const
{
    return m_sprites;
}

void ProjectLoader::start()
{
    if (m_loadThread.isRunning())
        m_loadThread.waitForFinished();

    if (m_loadStatus) {
        Q_ASSERT(m_engine);
        m_engine->start();
    }
}

void ProjectLoader::stop()
{
    if (m_loadThread.isRunning())
        m_loadThread.waitForFinished();

    if (m_loadStatus) {
        Q_ASSERT(m_engine);
        m_engine->stop();
    }
}

void ProjectLoader::timerEvent(QTimerEvent *event)
{
    if (m_loadThread.isRunning())
        return;

    if (m_engine)
        m_engine->step();

    event->accept();
}

void ProjectLoader::callLoad(ProjectLoader *loader)
{
    loader->load();
}

void ProjectLoader::load()
{
    m_loadStatus = m_project.load();
    m_engineMutex.lock();
    m_engine = m_project.engine().get();

    // Delete old sprites
    for (SpriteModel *sprite : m_sprites)
        sprite->deleteLater();

    m_sprites.clear();

    if (!m_engine || m_stopLoading) {
        m_engineMutex.unlock();
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

    auto handler = std::bind(&ProjectLoader::redraw, this);
    m_engine->setRedrawHandler(std::function<void()>(handler));

    // Load targets
    const auto &targets = m_engine->targets();

    for (auto target : targets) {
        if (target->isStage())
            dynamic_cast<Stage *>(target.get())->setInterface(&m_stage);
        else {
            SpriteModel *sprite = new SpriteModel;
            sprite->moveToThread(qApp->thread());
            dynamic_cast<Sprite *>(target.get())->setInterface(sprite);
            m_sprites.push_back(sprite);
        }
    }

    if (m_stopLoading) {
        m_engineMutex.unlock();
        emit fileNameChanged();
        emit loadStatusChanged();
        emit loadingFinished();
        emit engineChanged();
        emit spritesChanged();
        return;
    }

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
        m_loadThread.waitForFinished();

    // TODO: Call beforeRedraw() of targets
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

unsigned int ProjectLoader::downloadedAssets() const
{
    return m_downloadedAssets;
}

unsigned int ProjectLoader::assetCount() const
{
    return m_assetCount;
}
