/*
 * kwin_borders KWin plugin
 * SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
 * SPDX-License-Identifier: MIT
*/

#pragma once
#include <effect/effect.h>

namespace borderglow {
    namespace modules {

// Decides which windows qualify for the glow effect and under what conditions
// Accessed through singleton for glow eligibility across the plugin

        class GlowRules {
        public:
            bool canGlow(const KWin::EffectWindow* window, bool skipFullscreen, bool drawOnNormalWindows) const;
            static GlowRules& getInstance() {
                static GlowRules instance;
                return instance;
            }
        };
        
    }

    inline modules::GlowRules& glowRules(){
        return modules::GlowRules::getInstance();
    }
}