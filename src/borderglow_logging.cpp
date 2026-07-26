/*
 * SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
 * SPDX-License-Identifier: MIT
*/

#include "borderglow_logging.hpp"

// QtWarningMsg priority - skip QtDebugMsg and QtInfoMsg unless enabled via QT_LOGGING_RULES
Q_LOGGING_CATEGORY(KWIN_BORDERGLOW, "kwin_effect_borderglow", QtWarningMsg)