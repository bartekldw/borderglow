/*
 * kwin_borders KWin plugin
 * SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
 * SPDX-License-Identifier: MIT
*/

#pragma once 

namespace borderglow::modules {
    struct BehaviourProperties {
        bool skipFullscreen = true;
        bool drawOnNormalWindows = true;
        bool drawOnDialogs = true;
        bool drawOnDocks = false;
        bool drawOnPopups = false;
        bool drawOther = false;
    };

    struct UniformProperties {
        QVector4D color1 {};
        QVector4D color2 {};
        float radius = 0.f;
        float thickness = 0.f;
        float margin = 0.f;
    };
}