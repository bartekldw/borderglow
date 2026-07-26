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

    BorderGlow::BorderGlow() {
        qCDebug(KWIN_BORDERGLOW) << "[kwin_borders] Effect constructed - plugin loaded successfully";
    }

    BorderGlow::~BorderGlow() = default;

    bool BorderGlow::supported() {
        const auto glResult = effects->isOpenGLCompositing();

        if(glResult){
            qCDebug(KWIN_BORDERGLOW) << "[kwin_borders] [supported] OpenGL compositing available, effect supported";
        } else{
            qWarning() << "[kwin_borders] [supported] isOpenGLCompositing: OpenGL compositing available, effect unsupported";
        }

        return glResult;
    }

    bool BorderGlow::enabledByDefault() {
        return true;
    }
    
    KWIN_EFFECT_FACTORY(BorderGlow, "metadata.json")

}

#include "borderglow.moc"