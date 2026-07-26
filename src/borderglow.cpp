/*
 * SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
 * SPDX-License-Identifier: MIT
*/

/*    .--.
 *   |o_o |    kwin_borders
 *   |:_/ |    A customizable gradient border effect for KWin
 *  //   \ \
 * (|     | )  Plugin entry point: effect factory registration and
 * /'\_   _/`\ BorderGlow lifecycle
 * \___)=(___/
*/

#include <effect/effecthandler.h>

#include "borderglow.hpp"
#include "borderglow_logging.hpp"


namespace KWin {

    BorderGlow::BorderGlow() {}

    BorderGlow::~BorderGlow() {}

    bool BorderGlow::supported() {
        return effects->isOpenGLCompositing();
    }

    bool BorderGlow::enabledByDefault() {
        return true;
    }
    
    KWIN_EFFECT_FACTORY(BorderGlow, "metadata.json")

}

#include "borderglow.moc"