// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QOpenGLFramebufferObject>
#include <QOpenGLExtraFunctions>
#include <qnanopainter.h>
#include <scratchcpp/iengine.h>

#include "ipenlayer.h"
#include "texture.h"
#include "cputexturemanager.h"

namespace scratchcpprender
{

class PenLayer : public IPenLayer
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(libscratchcpp::IEngine *engine READ engine WRITE setEngine NOTIFY engineChanged)
        Q_PROPERTY(bool hqPen READ hqPen WRITE setHqPen NOTIFY hqPenChanged)

    public:
        PenLayer(QNanoQuickItem *parent = nullptr);
        ~PenLayer();

        bool antialiasingEnabled() const override;
        void setAntialiasingEnabled(bool enabled) override;

        libscratchcpp::IEngine *engine() const override;
        void setEngine(libscratchcpp::IEngine *newEngine) override;

        bool hqPen() const;
        void setHqPen(bool newHqPen);

        void clear() override;
        void drawPoint(const PenAttributes &penAttributes, double x, double y) override;
        void drawLine(const PenAttributes &penAttributes, double x0, double y0, double x1, double y1) override;
        void stamp(IRenderedTarget *target) override;

        QOpenGLFramebufferObject *framebufferObject() const override;
        QRgb colorAtScratchPoint(double x, double y) const override;

        const libscratchcpp::Rect &getBounds() const override;

        static IPenLayer *getProjectPenLayer(libscratchcpp::IEngine *engine);
        static void addPenLayer(libscratchcpp::IEngine *engine, IPenLayer *penLayer); // for tests

    signals:
        void engineChanged();
        void hqPenChanged();

    protected:
        QNanoQuickItemPainter *createItemPainter() const override;
        void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    private:
        void createFbo();
        void updateTexture();

        static std::unordered_map<libscratchcpp::IEngine *, IPenLayer *> m_projectPenLayers;
        static inline GLuint m_stampFbo = 0;
        bool m_antialiasingEnabled = true;
        libscratchcpp::IEngine *m_engine = nullptr;
        bool m_hqPen = false;
        std::unique_ptr<QOpenGLFramebufferObject> m_fbo;
        double m_scale = 1;
        std::unique_ptr<QNanoPainter> m_painter;
        std::unique_ptr<QOpenGLExtraFunctions> m_glF;
        Texture m_texture;
        bool m_textureDirty = true;
        mutable CpuTextureManager m_textureManager;
        mutable bool m_boundsDirty = true;
        mutable libscratchcpp::Rect m_bounds;
        GLuint m_vbo = 0;
        GLuint m_vao = 0;
};

} // namespace scratchcpprender
