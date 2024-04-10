// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QOpenGLFramebufferObject>
#include <QOpenGLPaintDevice>
#include <QOpenGLFunctions>
#include <QPainter>
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

    public:
        PenLayer(QNanoQuickItem *parent = nullptr);
        ~PenLayer();

        bool antialiasingEnabled() const override;
        void setAntialiasingEnabled(bool enabled) override;

        libscratchcpp::IEngine *engine() const override;
        void setEngine(libscratchcpp::IEngine *newEngine) override;

        void clear() override;
        void drawPoint(const PenAttributes &penAttributes, double x, double y) override;
        void drawLine(const PenAttributes &penAttributes, double x0, double y0, double x1, double y1) override;

        QOpenGLFramebufferObject *framebufferObject() const override;
        QRgb colorAtScratchPoint(double x, double y) const override;

        const libscratchcpp::Rect &getBounds() const override;

        static IPenLayer *getProjectPenLayer(libscratchcpp::IEngine *engine);
        static void addPenLayer(libscratchcpp::IEngine *engine, IPenLayer *penLayer); // for tests

    signals:
        void engineChanged();

    protected:
        QNanoQuickItemPainter *createItemPainter() const override;

    private:
        void updateTexture();

        static std::unordered_map<libscratchcpp::IEngine *, IPenLayer *> m_projectPenLayers;
        bool m_antialiasingEnabled = true;
        libscratchcpp::IEngine *m_engine = nullptr;
        std::unique_ptr<QOpenGLFramebufferObject> m_fbo;
        std::unique_ptr<QOpenGLFramebufferObject> m_resolvedFbo;
        std::unique_ptr<QOpenGLPaintDevice> m_paintDevice;
        QOpenGLFramebufferObjectFormat m_fboFormat;
        std::unique_ptr<QOpenGLFunctions> m_glF;
        Texture m_texture;
        bool m_textureDirty = true;
        mutable CpuTextureManager m_textureManager;
        mutable bool m_boundsDirty = true;
        mutable libscratchcpp::Rect m_bounds;
};

} // namespace scratchcpprender
