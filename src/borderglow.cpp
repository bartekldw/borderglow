/*
 * kwin_borders KWin plugin
 * SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
 * SPDX-License-Identifier: MIT
*/

/*    .--.
 *   |o_o |    kwin_borders
 *   |:_/ |    A customizable gradient border effect for KWin
 *  //   \ \
 * (|     | )  Plugin entry point: effect factory registration and
 * /'\_   _/`\ BorderGlow lifecycle
 * \___)=(___/
*/

#include <effect/effecthandler.h>
#include <core/renderviewport.h>
#include <opengl/glutils.h>

#include "borderglowsettings.h"
#include "borderglow.hpp"
#include "borderglow_logging.hpp"
#include "glowrules.hpp"
#include "utils.hpp"


namespace KWin {

// BorderGlow constructor Loads the border glow vertex and fragment shaders from the embedded Qt resources.
// Leaves m_shader null and logs a warning if compilation fails.

    BorderGlow::BorderGlow() {
        qCDebug(KWIN_BORDERGLOW) << "[kwin_borders] Effect constructed - plugin loaded successfully";

        m_shader = ShaderManager::instance()->generateShaderFromFile(
        ShaderTrait{},
        QStringLiteral(":/effects/borderglow/border.vert"),
        QStringLiteral(":/effects/borderglow/border.frag"));

        if (!m_shader) {
            qWarning() << "[kwin_borders] Shader compilation failed";
        }

        connect(effects, &EffectsHandler::windowClosed, this, &BorderGlow::slotWindowClosed);

        connect(effects, &EffectsHandler::windowDeleted, this, [this](EffectWindow *w) {
            const auto it = m_lastGeometry.constFind(w);
            if (it != m_lastGeometry.cend()) {
                w->addLayerRepaint(it.value());
                m_lastGeometry.erase(it);
            }
        });

        connect(effects, &EffectsHandler::desktopChanged, this, [](VirtualDesktop *, VirtualDesktop *, EffectWindow *) {
            effects->addRepaintFull();
        });

        reconfigure(ReconfigureAll);
    }

    BorderGlow::~BorderGlow() = default;

    void BorderGlow::slotWindowClosed(EffectWindow *w) {
        m_lastGeometry.remove(w);
    }

// Reloads plugin settings from the config file and applies them to internal state.
// Repaints the screen to reflect any visual changes

    void BorderGlow::reconfigure(ReconfigureFlags flags) {
        Q_UNUSED(flags)
        
        BorderGlowSettings::self()->config()->reparseConfiguration();
        BorderGlowSettings::self()->read();
     
        m_behaviourProperties.skipFullscreen = BorderGlowSettings::skipFullscreen();
        m_behaviourProperties.drawOnNormalWindows = BorderGlowSettings::drawNormalWindows();
        m_behaviourProperties.drawOnDialogs = BorderGlowSettings::drawDialogs();
        m_behaviourProperties.drawOnDocks = BorderGlowSettings::drawDocks();
        m_behaviourProperties.drawOnPopups = BorderGlowSettings::drawPopups();
        m_behaviourProperties.drawOther = BorderGlowSettings::drawOther();

        m_uniformProperties.color1 = borderglow::modules::QColorToVec4(BorderGlowSettings::gradientColor1());
        m_uniformProperties.color2 = borderglow::modules::QColorToVec4(BorderGlowSettings::gradientColor2());
        m_uniformProperties.radius = BorderGlowSettings::borderRadius();
        m_uniformProperties.thickness = BorderGlowSettings::borderThickness();
        m_uniformProperties.margin = m_uniformProperties.thickness;
        
        effects->addRepaintFull();
    }

// Reports whether the effect can run: it requires an OpenGL compositing backend.
// The result is logged at debug level on success and as a warning otherwise

    bool BorderGlow::supported() {
        const auto glResult = effects->isOpenGLCompositing();

        if(glResult){
            qCDebug(KWIN_BORDERGLOW) << "[kwin_borders] [supported] OpenGL compositing available, effect supported";
        } else{
            qWarning() << "[kwin_borders] [supported] isOpenGLCompositing: OpenGL compositing unavailable, effect unsupported";
        }

        return glResult;
    }

    bool BorderGlow::enabledByDefault() {
        return true;
    }
    
// Expands the repaint region to cover the glow margin around the window, repainting
// both the current and previous frame geometry so stale border pixels are cleared

    void BorderGlow::prePaintWindow(RenderView *view, EffectWindow *w, WindowPrePaintData &data) {
        if (m_shader && borderglow::glowRules().canGlow(w, m_behaviourProperties)) {
            const qreal m = m_uniformProperties.margin;
            const QRectF current = w->frameGeometry().adjusted(-m, -m, m, m);

            w->addLayerRepaint(current);
            const auto it = m_lastGeometry.constFind(w);
            if (it != m_lastGeometry.cend() && it.value() != current) {
                w->addLayerRepaint(it.value());
            }
            m_lastGeometry[w] = current;

            data.setTranslucent();
        } else {
            m_lastGeometry.remove(w);
        }

        effects->prePaintWindow(view, w, data);
    }

// Draws the window using KWin API, then renders the glow border as a screen-space quad expanded by margin, following 
// the window's current transform (scale, translation) so the border stays attached during animations

    void BorderGlow::paintWindow(const RenderTarget &renderTarget, const RenderViewport &viewport,
                                   EffectWindow *w, int mask, const Region &deviceRegion, WindowPaintData &data) {

        effects->paintWindow(renderTarget, viewport, w, mask, deviceRegion, data);
        
        if(!borderglow::glowRules().canGlow(w, m_behaviourProperties)){
            return;
        }

        if (!m_shader) {
            return;
        }

        const qreal dpr = viewport.scale();

        const QRectF windowGeo = w->frameGeometry();

        const qreal marginX = data.xScale() > 0.0001 ? m_uniformProperties.margin / data.xScale() : m_uniformProperties.margin;
        const qreal marginY = data.yScale() > 0.0001 ? m_uniformProperties.margin / data.yScale() : m_uniformProperties.margin;

        const qreal quadW = windowGeo.width() + 2.0 * marginX;
        const qreal quadH = windowGeo.height() + 2.0 * marginY;

        const RectF maximizeArea = effects->clientArea(MaximizeArea, w);

        constexpr qreal kEpsilon = 0.5;
        constexpr qreal kScaleEpsilon = 0.001;

        // CSD can report a stuck maximized frameGeometry() while actually being rendered smaller/offset via the paint transform
        // Excluding transformed windows avoids the false positive

        const bool geometryMatches = qAbs(windowGeo.width()  - maximizeArea.width())  <= kEpsilon && qAbs(windowGeo.height() - maximizeArea.height()) <= kEpsilon;
        const bool isBeingScaled = qAbs(data.xScale() - 1.0) > kScaleEpsilon || qAbs(data.yScale() - 1.0) > kScaleEpsilon;
        const bool maximized = geometryMatches && !isBeingScaled;
        const float radius = maximized ? 0.0f : m_uniformProperties.radius;

        const bool isAnimating = isBeingScaled || !qFuzzyIsNull(data.xTranslation()) || !qFuzzyIsNull(data.yTranslation());

        if (isAnimating) {
            const qreal m = m_uniformProperties.margin;
            QRectF painted(windowGeo.x() + data.xTranslation(), windowGeo.y() + data.yTranslation(), windowGeo.width()  * data.xScale(), windowGeo.height() * data.yScale());
            painted.adjust(-m, -m, m, m);
            effects->addRepaint(RectF(painted));
        }

        QMatrix4x4 mvp = viewport.projectionMatrix();
        mvp.translate(windowGeo.x() * dpr, windowGeo.y() * dpr);
        mvp.translate(data.xTranslation(), data.yTranslation(), data.zTranslation());
        mvp.scale(data.xScale(), data.yScale(), data.zScale());
        mvp.scale(dpr, dpr, 1.0);
        mvp.translate(-marginX, -marginY);

        ShaderManager *sm = ShaderManager::instance();
        sm->pushShader(m_shader.get());

        m_shader->setUniform("modelViewProjectionMatrix", mvp);
        m_shader->setUniform("u_size", QVector2D(quadW, quadH));
        m_shader->setUniform("u_inner_size", QVector2D(windowGeo.width(), windowGeo.height()));
        m_shader->setUniform("u_radius", radius);
        m_shader->setUniform("u_border", m_uniformProperties.thickness);

        const float finalAlpha = 1.0f * static_cast<float>(data.opacity());
        m_shader->setUniform("u_color1", QVector4D(m_uniformProperties.color1[0], m_uniformProperties.color1[1], m_uniformProperties.color1[2], m_uniformProperties.color1[3] * finalAlpha));
        m_shader->setUniform("u_color2", QVector4D(m_uniformProperties.color2[0], m_uniformProperties.color2[1], m_uniformProperties.color2[2], m_uniformProperties.color2[3] * finalAlpha));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GLVertexBuffer *vbo = GLVertexBuffer::streamingBuffer();
        vbo->reset();
        vbo->setAttribLayout(std::span(GLVertexBuffer::GLVertex2DLayout), sizeof(GLVertex2D));

        QVector<GLVertex2D> verts;
        verts.append({QVector2D(0, 0), QVector2D(0, 0)});
        verts.append({QVector2D(quadW, 0), QVector2D(1, 0)});
        verts.append({QVector2D(quadW, quadH), QVector2D(1, 1)});
        verts.append({QVector2D(0, 0), QVector2D(0, 0)});
        verts.append({QVector2D(quadW, quadH), QVector2D(1, 1)});
        verts.append({QVector2D(0, quadH), QVector2D(0, 1)});

        vbo->setVertices(verts);
        vbo->render(GL_TRIANGLES);

        glDisable(GL_BLEND);

        sm->popShader();
    }

    
// Registers BorderGlow as a loadable KWin effect plugin, using metadata.json for its ID and description
    KWIN_EFFECT_FACTORY(BorderGlow, "metadata.json")

}

#include "borderglow.moc"