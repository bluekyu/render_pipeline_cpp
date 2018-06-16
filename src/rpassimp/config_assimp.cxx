/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file config_assimp.cxx
 * @author rdb
 * @date 2011-03-29
 */

#include "config_assimp.h"

#include <dconfig.h>
#include <loaderFileTypeRegistry.h>

#include "loaderFileTypeAssimp.h"

Configure(config_rpassimp);
NotifyCategoryDef(rpassimp, "");

ConfigureFn(config_rpassimp)
{
    static bool initialized = false;
    if (initialized)
        return;
    initialized = true;

    rpassimp::LoaderFileTypeAssimp::init_type();

    LoaderFileTypeRegistry *reg = LoaderFileTypeRegistry::get_global_ptr();
    reg->register_type(new rpassimp::LoaderFileTypeAssimp);
}
