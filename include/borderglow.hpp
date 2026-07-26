/*
 * kwin_borders KWin plugin
 * SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
 * SPDX-License-Identifier: MIT
*/

#pragma once

#include <effect/effect.h>
#include <QObject>

namespace KWin {

    class BorderGlow : public Effect {
        Q_OBJECT

    public:
        BorderGlow();
        ~BorderGlow() override;

        static bool supported();
        static bool enabledByDefault();
    };

}