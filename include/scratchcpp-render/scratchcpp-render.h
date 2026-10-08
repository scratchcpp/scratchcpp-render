// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define DECL_EXPORT __declspec(dllexport)
#define DECL_IMPORT __declspec(dllimport)
#else
#define DECL_EXPORT __attribute__((visibility("default")))
#define DECL_IMPORT __attribute__((visibility("default")))
#endif

#if defined(SCRATCHCPPRENDER_LIBRARY)
#define SCRATCHCPPRENDER_EXPORT DECL_EXPORT
#else
#define SCRATCHCPPRENDER_EXPORT DECL_IMPORT
#endif

#include <string>

/*! \brief The main namespace of the library. */
namespace scratchcpprender
{

/*! Initializes the library. Call this from main before constructing your Q(Gui)Application object. */
void init();

/*! Returns the version string of the library. */
const std::string &version();

/*! Returns the major version of the library. */
int majorVersion();

/*! Returns the minor version of the library. */
int minorVersion();

/*! Returns the patch version of the library. */
int patchVersion();

} // namespace scratchcpprender
