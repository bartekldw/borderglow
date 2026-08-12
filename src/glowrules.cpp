#include "glowrules.hpp"
#include <effect/effecthandler.h>

namespace borderglow::modules {

// Determine if border should be displayed on given KWin::EffectWindow depending on its state
// TODO: Will be driven by kcfg module to configure on which windows should border be drawn

    bool GlowRules::canGlow(const KWin::EffectWindow* window) const {
        if (window->isDesktop() || window->isLockScreen() || window->isDNDIcon() || window->isOutline()) {
            return false;
        }
        return true;
    }
}