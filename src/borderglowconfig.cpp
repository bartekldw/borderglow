#include "borderglowconfig.hpp"
#include "borderglowsettings.h"
#include <KPluginFactory>

#include <QDBusConnection>
#include <QDBusMessage>

namespace KWin {

    K_PLUGIN_CLASS(BorderGlowConfig)

    BorderGlowConfig::BorderGlowConfig(QObject *parent, const KPluginMetaData &data) : KCModule(parent, data) {
        m_ui.setupUi(widget());
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