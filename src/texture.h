// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QtOpenGL>

namespace scratchcpprender
{

class Texture
{
    public:
        Texture();
        Texture(GLuint texture, const QSize &size);
        Texture(GLuint texture, int width, int height);

        GLuint handle() const;
        bool isValid() const;
        const QSize &size() const;
        int width() const;
        int height() const;

        QImage toImage() const;

        void release();

        bool operator==(const Texture &texture) const;
        bool operator!=(const Texture &texture) const;

    private:
        GLuint m_handle = 0;
        bool m_isValid = false;
        QSize m_size;
};

} // namespace scratchcpprender
