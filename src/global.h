// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QtCore/qglobal.h>

#if defined(SCRATCHCPPGUI_LIBRARY)
#define SCRATCHCPPGUI_EXPORT Q_DECL_EXPORT
#else
#define SCRATCHCPPGUI_EXPORT Q_DECL_IMPORT
#endif
