// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/iengine.h>
#include <scratchcpp/value.h>
#include <QtConcurrent/QtConcurrent>
#include <QApplication>

#include "projectloader.h"
#include "spritemodel.h"
#include "renderedtarget.h"

using namespace scratchcppgui;
using namespace libscratchcpp;

void runEventLoop(IEngine *engine)
{
    engine->runEventLoop();
}

ProjectLoader::ProjectLoader(QObject *parent) :
    QObject(parent)
{
    initTimer();

    // Update refresh rate when primary screen changes
    connect(qApp, &QApplication::primaryScreenChanged, this, [this]() {
        killTimer(m_timerId);
        initTimer();
    });
}

ProjectLoader::~ProjectLoader()
{
    if (m_loadThread.isRunning())
        m_loadThread.waitForFinished();

    if (m_engine) {
        m_engine->stopEventLoop();
        m_eventLoop.waitForFinished();
    }

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

    if (m_engine) {
        m_engine->stopEventLoop();
        m_eventLoop.waitForFinished();
    }

    m_project.setScratchVersion(ScratchVersion::Scratch3);
    m_project.setFileName(m_fileName.toStdString());
    m_loadStatus = false;
    emit loadStatusChanged();
    emit fileNameChanged();

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

    auto stageRenderedTarget = m_stage.renderedTarget();

    if (stageRenderedTarget)
        stageRenderedTarget->updateProperties();

    for (auto sprite : m_sprites) {
        auto renderedTarget = sprite->renderedTarget();

        if (renderedTarget)
            renderedTarget->updateProperties();
    }

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

    if (!m_engine) {
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

    auto handler = std::bind(&ProjectLoader::emitTick, this);
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

    // Run event loop
    m_engine->setSpriteFencingEnabled(false);

    if (m_eventLoopEnabled)
        m_eventLoop = QtConcurrent::run(&runEventLoop, m_engine);

    m_engineMutex.unlock();

    emit loadStatusChanged();
    emit loadingFinished();
    emit engineChanged();
    emit stageChanged();
    emit spritesChanged();
}

void ProjectLoader::initTimer()
{
    QScreen *screen = qApp->primaryScreen();

    if (screen)
        m_timerId = startTimer(1000 / screen->refreshRate());
}

void ProjectLoader::emitTick()
{
    if (m_loadThread.isRunning())
        m_loadThread.waitForFinished();

    auto stageRenderedTarget = m_stage.renderedTarget();

    if (stageRenderedTarget)
        stageRenderedTarget->loadProperties();

    for (auto sprite : m_sprites) {
        auto renderedTarget = sprite->renderedTarget();

        if (renderedTarget)
            renderedTarget->loadProperties();
    }
}

double ProjectLoader::fps() const
{
    return m_fps;
}

void ProjectLoader::setFps(double newFps)
{
    if (qFuzzyCompare(m_fps, newFps))
        return;

    m_fps = newFps;
    m_engineMutex.lock();

    if (m_engine)
        m_engine->setFps(m_fps);

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

    m_cloneLimit = newCloneLimit;
    m_engineMutex.lock();

    if (m_engine)
        m_engine->setCloneLimit(m_cloneLimit);

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

bool ProjectLoader::eventLoopEnabled() const
{
    return m_eventLoopEnabled;
}

void ProjectLoader::setEventLoopEnabled(bool newEventLoopEnabled)
{
    if (m_eventLoopEnabled == newEventLoopEnabled)
        return;

    m_eventLoopEnabled = newEventLoopEnabled;
    m_engineMutex.lock();

    if (m_engine) {
        if (m_eventLoopEnabled)
            m_eventLoop = QtConcurrent::run(&runEventLoop, m_engine);
        else {
            m_engine->stopEventLoop();
            m_eventLoop.waitForFinished();
        }
    }

    m_engineMutex.unlock();
    emit eventLoopEnabledChanged();
}
