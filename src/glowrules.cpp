/*
 * kwin_borders KWin plugin
 * SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
 * SPDX-License-Identifier: MIT
*/

#include <effect/effecthandler.h>
#include "glowrules.hpp"


namespace borderglow::modules {

// Determine if border should be displayed on given KWin::EffectWindow depending on its state
// TODO: Will be driven by kcfg module to configure on which windows should border be drawn

    bool GlowRules::canGlow(const KWin::EffectWindow* window, bool skipFullscreen, bool drawOnNormalWindows, bool drawOnDialogs) const {
        if (window->isDesktop() || window->isLockScreen() || window->isDNDIcon() || window->isOutline()) {
            return false;
        }

        if(skipFullscreen && window->isFullScreen()) {
            return false;
        }

        if (window->isNormalWindow()) {
            return drawOnNormalWindows;
        }

        if (window->isDialog() || window->isUtility()) {
            return drawOnDialogs;
        }

        return true;
    }
}