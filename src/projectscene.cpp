// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QQuickWindow>
#include <scratchcpp/iengine.h>
#include <scratchcpp/keyevent.h>

#include "projectscene.h"
#include "keyeventhandler.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

static const std::unordered_map<Qt::Key, KeyEvent::Type> SPECIAL_KEY_MAP = {
    { Qt::Key_Space, KeyEvent::Type::Space }, { Qt::Key_Left, KeyEvent::Type::Left },    { Qt::Key_Up, KeyEvent::Type::Up },      { Qt::Key_Right, KeyEvent::Type::Right },
    { Qt::Key_Down, KeyEvent::Type::Down },   { Qt::Key_Return, KeyEvent::Type::Enter }, { Qt::Key_Enter, KeyEvent::Type::Enter }
};

ProjectScene::ProjectScene(QQuickItem *parent)
{
    m_keyHandler = new KeyEventHandler(this);
    connect(this, &QQuickItem::windowChanged, this, &ProjectScene::installKeyHandler);
    connect(m_keyHandler, &KeyEventHandler::keyPressed, this, &ProjectScene::handleKeyPress);
    connect(m_keyHandler, &KeyEventHandler::keyReleased, this, &ProjectScene::handleKeyRelease);
}

IEngine *ProjectScene::engine() const
{
    return m_engine;
}

void ProjectScene::setEngine(IEngine *newEngine)
{
    if (m_engine == newEngine)
        return;

    m_engine = newEngine;
    emit engineChanged();
}

double ProjectScene::stageScale() const
{
    return m_stageScale;
}

void ProjectScene::setStageScale(double newStageScale)
{
    if (qFuzzyCompare(m_stageScale, newStageScale))
        return;

    m_stageScale = newStageScale;
    emit stageScaleChanged();
}

void ProjectScene::handleMouseMove(qreal x, qreal y)
{
    if (m_engine) {
        m_engine->setMouseX(x / m_stageScale - m_engine->stageWidth() / 2.0);
        m_engine->setMouseY(-y / m_stageScale + m_engine->stageHeight() / 2.0);
    }
}

void ProjectScene::handleMousePress()
{
    if (m_engine)
        m_engine->setMousePressed(true);
}

void ProjectScene::handleMouseRelease()
{
    if (m_engine)
        m_engine->setMousePressed(false);
}

void ProjectScene::handleKeyPress(Qt::Key key, const QString &text)
{
    m_pressedKeys.insert(key);

    if (m_engine) {
        auto it = SPECIAL_KEY_MAP.find(key);

        if (it == SPECIAL_KEY_MAP.cend()) {
            if (!text.isEmpty())
                m_engine->setKeyState(text.toStdString(), true);
        } else {
            KeyEvent event(it->second);
            // TODO: Use event instead of even.name()
            m_engine->setKeyState(event.name(), true);
        }

        m_engine->setAnyKeyPressed(!m_pressedKeys.empty());
    }
}

void ProjectScene::handleKeyRelease(Qt::Key key, const QString &text)
{
    m_pressedKeys.erase(key);

    if (m_engine) {
        auto it = SPECIAL_KEY_MAP.find(key);

        if (it == SPECIAL_KEY_MAP.cend()) {
            if (!text.isEmpty())
                m_engine->setKeyState(text.toStdString(), false);
        } else {
            KeyEvent event(it->second);
            // TODO: Use event instead of even.name()
            m_engine->setKeyState(event.name(), false);
        }

        if (m_pressedKeys.empty()) // avoid setting 'true' when a key is released
            m_engine->setAnyKeyPressed(false);
    }
}

void ProjectScene::installKeyHandler(QQuickWindow *window)
{
    if (window)
        window->installEventFilter(m_keyHandler);
}
