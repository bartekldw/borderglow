/*
 * kwin_borders KWin plugin
 * SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
 * SPDX-License-Identifier: MIT
*/

#pragma once 

#include <QColor>
#include <QVector4D>

namespace borderglow::modules {

    [[nodiscard]] inline QVector4D QColorToVec4(const QColor &color) {
        return QVector4D(
            static_cast<float>(color.redF()),
            static_cast<float>(color.greenF()),
            static_cast<float>(color.blueF()),
            static_cast<float>(color.alphaF())
        );
    }

}