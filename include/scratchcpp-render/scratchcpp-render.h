// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

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
