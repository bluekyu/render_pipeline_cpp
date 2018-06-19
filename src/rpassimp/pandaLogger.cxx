/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file pandaLogger.cxx
 * @author rdb
 * @date 2011-05-05
 */

#include "pandaLogger.h"

#include <assimp/DefaultLogger.hpp>

#include "config_assimp.h"

namespace rpassimp {

PandaLogger* PandaLogger::_ptr = nullptr;

/**
 * Makes sure there's a global PandaLogger object and makes sure that it is
 * Assimp's default logger.
 */
void PandaLogger::set_default() {
  if (_ptr == nullptr) {
    _ptr = new PandaLogger;
  }
  if (_ptr != Assimp::DefaultLogger::get()) {
    Assimp::DefaultLogger::set(_ptr);
  }
}

/**
 *
 */
void PandaLogger::OnDebug(const char *message) {
  rpassimp_cat.debug() << "[Assimp] " << message << std::endl;
}

/**
 *
 */
void PandaLogger::OnError(const char *message) {
  rpassimp_cat.error() << "[Assimp] " << message << std::endl;
}

/**
 *
 */
void PandaLogger::OnInfo(const char *message) {
  rpassimp_cat.info() << "[Assimp] " << message << std::endl;
}

/**
 *
 */
void PandaLogger::OnWarn(const char *message) {
  rpassimp_cat.warning() << "[Assimp] " << message << std::endl;
}

}
