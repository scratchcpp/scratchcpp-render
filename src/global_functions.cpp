// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QQuickWindow>
#include <scratchcpp-render/scratchcpp-render.h>
#include <scratchcpp/scratchconfiguration.h>

void scratchcpprender::init()
{
    qputenv("QSG_RENDER_LOOP", "basic");
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setSwapInterval(0);
#ifdef Q_OS_MACOS
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3, 2);
#endif
    QSurfaceFormat::setDefaultFormat(format);
}

const std::string &scratchcpprender::version()
{
    static const std::string ret = SCRATCHCPPRENDER_VERSION;
    return ret;
}

int scratchcpprender::majorVersion()
{
    return SCRATCHCPPRENDER_VERSION_MAJOR;
}

int scratchcpprender::minorVersion()
{
    return SCRATCHCPPRENDER_VERSION_MINOR;
}

int scratchcpprender::patchVersion()
{
    return SCRATCHCPPRENDER_VERSION_PATCH;
}

const std::string &scratchcpprender::libscratchcppVersion()
{
    return libscratchcpp::ScratchConfiguration::version();
}

int scratchcpprender::libscratchcppMajorVersion()
{
    return libscratchcpp::ScratchConfiguration::majorVersion();
}

int scratchcpprender::libscratchcppMinorVersion()
{
    return libscratchcpp::ScratchConfiguration::minorVersion();
}

int scratchcpprender::libscratchcppPatchVersion()
{
    return libscratchcpp::ScratchConfiguration::patchVersion();
}
