/*
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
        
        m_skipFullscreen = BorderGlowSettings::skipFullscreen();
        
        QColor color1 = BorderGlowSettings::gradientColor1();
        QColor color2 = BorderGlowSettings::gradientColor2();

        m_color1 = QVector4D(
            static_cast<float>(color1.redF()),
            static_cast<float>(color1.greenF()),
            static_cast<float>(color1.blueF()),
            static_cast<float>(color1.alphaF())
        );
        m_color2 = QVector4D(
            static_cast<float>(color2.redF()),
            static_cast<float>(color2.greenF()),
            static_cast<float>(color2.blueF()),
            static_cast<float>(color2.alphaF())
        );

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
        const QRectF current = w->frameGeometry().adjusted(
            -BORDER_MARGIN, -BORDER_MARGIN, BORDER_MARGIN, BORDER_MARGIN);

        w->addLayerRepaint(current);
        if (m_lastGeometry.contains(w)) {
            w->addLayerRepaint(m_lastGeometry[w]);
        }
        m_lastGeometry[w] = current;
        effects->prePaintWindow(view, w, data);
    }

// Draws the window using KWin API, then renders the glow border as a screen-space quad expanded by margin, following 
// the window's current transform (scale, translation) so the border stays attached during animations

    void BorderGlow::paintWindow(const RenderTarget &renderTarget, const RenderViewport &viewport,
                                   EffectWindow *w, int mask, const Region &deviceRegion, WindowPaintData &data) {

        effects->paintWindow(renderTarget, viewport, w, mask, deviceRegion, data);
        
        if(!borderglow::glowRules().canGlow(w, m_skipFullscreen)){
            return;
        }

        if (!m_shader) {
            return;
        }

        const QRectF windowGeo = w->frameGeometry();

        const qreal marginX = data.xScale() > 0.0001 ? BORDER_MARGIN / data.xScale() : BORDER_MARGIN;
        const qreal marginY = data.yScale() > 0.0001 ? BORDER_MARGIN / data.yScale() : BORDER_MARGIN;

        const qreal quadW = windowGeo.width() + 2.0 * marginX;
        const qreal quadH = windowGeo.height() + 2.0 * marginY;

        const RectF maximizeArea = effects->clientArea(MaximizeArea, w);
        const bool maximized = (windowGeo.width() >= maximizeArea.width() - 1.0) &&
                            (windowGeo.height() >= maximizeArea.height() - 1.0);
        const float radius = maximized ? 0.0f : BORDER_RADIUS;
        
        QMatrix4x4 mvp = viewport.projectionMatrix();
        mvp.translate(windowGeo.x(), windowGeo.y());
        mvp.translate(data.xTranslation(), data.yTranslation(), data.zTranslation());
        mvp.scale(data.xScale(), data.yScale(), data.zScale());
        mvp.translate(-marginX, -marginY);

        ShaderManager *sm = ShaderManager::instance();
        sm->pushShader(m_shader.get());

        m_shader->setUniform("modelViewProjectionMatrix", mvp);
        m_shader->setUniform("u_size", QVector2D(quadW, quadH));
        m_shader->setUniform("u_inner_size", QVector2D(windowGeo.width(), windowGeo.height()));
        m_shader->setUniform("u_radius", radius);
        m_shader->setUniform("u_border", BORDER_THICKNESS);

        const float finalAlpha = 1.0f * static_cast<float>(data.opacity());
        m_shader->setUniform("u_color1", QVector4D(m_color1[0], m_color1[1], m_color1[2], m_color1[3] * finalAlpha));
        m_shader->setUniform("u_color2", QVector4D(m_color2[0], m_color2[1], m_color2[2], m_color2[3] * finalAlpha));

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