/*
 * kwin_borders KWin plugin
 * SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
 * SPDX-License-Identifier: MIT
*/

#pragma once

#include <memory>
#include <opengl/glshader.h>
#include <effect/effect.h>
#include <QObject>
#include <QHash>

namespace KWin {

    inline constexpr float BORDER_MARGIN = 4.0f;
    inline constexpr float BORDER_THICKNESS = 2.0f;
    inline constexpr float BORDER_RADIUS = 8.0f;

    inline constexpr float BORDER_COLOR_GRAD1[4] = {0.55f, 0.15f, 0.95f, 1.0f};
    inline constexpr float BORDER_COLOR_GRAD2[4] = {0.0f, 0.9f, 1.0f, 1.0f};


// Core rendering effect for kwin_borders - paints a glowing SDF-based
// border around windows driven by a custom GLSL shader

    class BorderGlow : public Effect {
        Q_OBJECT

    private:
        QHash<EffectWindow *, QRectF> m_lastGeometry;
        std::unique_ptr<GLShader> m_shader;

    public:

        BorderGlow();
        ~BorderGlow() override;

        void prePaintWindow(RenderView *view, EffectWindow *w, WindowPrePaintData &data) override;
        void paintWindow(const RenderTarget &renderTarget, const RenderViewport &viewport,
                     EffectWindow *w, int mask, const Region &deviceRegion, WindowPaintData &data) override;
        static bool supported();
        static bool enabledByDefault();
    };

}