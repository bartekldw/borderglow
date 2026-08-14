/*
 * kwin_borders KWin plugin
 * SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
 * SPDX-License-Identifier: MIT
*/

#include <KPluginFactory>
#include <QDBusConnection>
#include <QDBusMessage>

#include "borderglowconfig.hpp"
#include "borderglowsettings.h"


namespace KWin {

    K_PLUGIN_CLASS(BorderGlowConfig)

    BorderGlowConfig::BorderGlowConfig(QObject *parent, const KPluginMetaData &data) : KCModule(parent, data) {
        m_ui.setupUi(widget());

        connect(m_ui.kcfg_BorderRadius, &QSlider::valueChanged, m_ui.borderRadiusPreview, &QSpinBox::setValue);
        connect(m_ui.borderRadiusPreview, QOverload<int>::of(&QSpinBox::valueChanged), m_ui.kcfg_BorderRadius, &QSlider::setValue);
        m_ui.borderRadiusPreview->setValue(m_ui.kcfg_BorderRadius->value());
        
        connect(m_ui.kcfg_BorderThickness, &QSlider::valueChanged, m_ui.borderThicknessPreview, &QSpinBox::setValue);
        connect(m_ui.borderThicknessPreview, QOverload<int>::of(&QSpinBox::valueChanged), m_ui.kcfg_BorderThickness, &QSlider::setValue);
        m_ui.borderThicknessPreview->setValue(m_ui.kcfg_BorderThickness->value());

        addConfig(BorderGlowSettings::self(), widget());
    }

    void BorderGlowConfig::save() {
        KCModule::save();
        QDBusMessage message = QDBusMessage::createMethodCall(
            QStringLiteral("org.kde.KWin"),
            QStringLiteral("/Effects"),
            QStringLiteral("org.kde.kwin.Effects"),
            QStringLiteral("reconfigureEffect"));
        message.setArguments({QStringLiteral("borderglow_alpha")});
        QDBusConnection::sessionBus().send(message);
    }

}

#include "borderglowconfig.moc"