/*
 * kwin_borders KWin plugin
 * SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
 * SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <effect/effecthandler.h>
#include "glowrules.hpp"


namespace borderglow::modules {

// Determine if border should be displayed on given KWin::EffectWindow depending on its state
// TODO: Will be driven by kcfg module to configure on which windows should border be drawn

    bool GlowRules::canGlow(const KWin::EffectWindow* window, const BehaviourProperties& properties) const {
        if (window->isDesktop() || window->isLockScreen() || window->isDNDIcon() || window->isOutline()) {
            return false;
        }

        if(properties.skipFullscreen && window->isFullScreen()) {
            return false;
        }

        if (window->isNormalWindow()) {
            return properties.drawOnNormalWindows;
        }

        if (window->isDialog() || window->isUtility()) {
            return properties.drawOnDialogs;
        }

        if (window->isDock()) {
            return properties.drawOnDocks;
        }

        if (window->isPopupWindow() || window->isPopupMenu() || window->isDropdownMenu() || window->isComboBox() || window->isMenu() || window->isTooltip() || 
            window->isAppletPopup() || window->isNotification() || window->isOnScreenDisplay() || window->isCriticalNotification()) {
            return properties.drawOnPopups;
        }

        return properties.drawOther;
    }
}