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
    if (m_engine) {
        m_engine->stopEventLoop();
        m_eventLoop.waitForFinished();
    }
}

const QString &ProjectLoader::fileName() const
{
    return m_fileName;
}

void ProjectLoader::setFileName(const QString &newFileName)
{
    if (m_fileName == newFileName)
        return;

    m_fileName = newFileName;

    if (m_engine) {
        m_engine->stopEventLoop();
        m_eventLoop.waitForFinished();
    }

    m_project.setScratchVersion(ScratchVersion::Scratch3);
    m_project.setFileName(m_fileName.toStdString());
    m_loaded = m_project.load();
    m_engine = m_project.engine().get();

    m_engine->setFps(m_fps);

    auto handler = std::bind(&ProjectLoader::emitTick, this);
    m_engine->setRedrawHandler(std::function<void()>(handler));

    // Delete old sprites
    for (SpriteModel *sprite : m_sprites)
        sprite->deleteLater();

    m_sprites.clear();

    // Load targets
    const auto &targets = m_engine->targets();

    for (auto target : targets) {
        if (target->isStage())
            dynamic_cast<Stage *>(target.get())->setInterface(&m_stage);
        else {
            SpriteModel *sprite = new SpriteModel(this);
            dynamic_cast<Sprite *>(target.get())->setInterface(sprite);
            m_sprites.push_back(sprite);
        }
    }

    // Run event loop
    m_engine->setSpriteFencingEnabled(false);
    m_eventLoop = QtConcurrent::run(&runEventLoop, m_engine);

    emit fileNameChanged();
    emit loadedChanged();
    emit engineChanged();
    emit stageChanged();
    emit spritesChanged();
}

bool ProjectLoader::loaded() const
{
    return m_loaded;
}

IEngine *ProjectLoader::engine() const
{
    return m_engine;
}

StageModel *ProjectLoader::stage()
{
    return &m_stage;
}

QQmlListProperty<SpriteModel> ProjectLoader::sprites()
{
    return QQmlListProperty<SpriteModel>(this, &m_sprites);
}

void ProjectLoader::start()
{
    m_engine->start();
}

void ProjectLoader::stop()
{
    m_engine->stop();
}

void ProjectLoader::timerEvent(QTimerEvent *event)
{
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

void ProjectLoader::initTimer()
{
    QScreen *screen = qApp->primaryScreen();

    if (screen)
        m_timerId = startTimer(1000 / screen->refreshRate());
}

void ProjectLoader::emitTick()
{
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

    if (m_engine)
        m_engine->setFps(m_fps);

    emit fpsChanged();
}
