/*
 * kwin_borders KWin plugin
 * SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
 * SPDX-License-Identifier: MIT
*/

#pragma once

#include <KCModule>
#include <KPluginMetaData>

#include "ui_borderglowconfigform.h"

namespace KWin {
    class BorderGlowConfig : public KCModule {
        Q_OBJECT

    private:
        Ui::BorderGlowConfigForm m_ui;

    public:
        explicit BorderGlowConfig(QObject *parent, const KPluginMetaData &data);
        void save() override;
    };

}
