// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QVector2D>

#include "effecttransform.h"

using namespace scratchcpprender;

// A texture coordinate is between 0 and 1, 0.5 is the center position
static const float CENTER_X = 0.5f;
static const float CENTER_Y = 0.5f;

inline float fract(float x)
{
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/fract.xhtml
    return x - std::floor(x);
}

QRgb EffectTransform::transformColor(ShaderManager::Effect effectMask, const std::unordered_map<ShaderManager::Effect, double> &effectValues, QRgb color)
{
    // https://github.com/scratchfoundation/scratch-render/blob/e075e5f5ebc95dec4a2718551624ad587c56f0a6/src/EffectTransform.js#L40-L119
    // If the color is fully transparent, don't bother attempting any transformations.
    if (qAlpha(color) == 0)
        return color;

    QColor inOutColor = QColor::fromRgba(color);

    std::unordered_map<ShaderManager::Effect, float> uniforms;
    ShaderManager::getUniformValuesForEffects(effectValues, uniforms);

    const bool enableColor = (effectMask & ShaderManager::Effect::Color) != 0;
    const bool enableBrightness = (effectMask & ShaderManager::Effect::Brightness) != 0;

    if (enableColor || enableBrightness) {
        // gl_FragColor.rgb /= gl_FragColor.a + epsilon;
        // Here, we're dividing by the (previously pre-multiplied) alpha to ensure HSV is properly calculated
        // for partially transparent pixels.
        const float alpha = inOutColor.alphaF();

        if (alpha == 0) {
            inOutColor.setRed(255);
            inOutColor.setGreen(255);
            inOutColor.setBlue(255);
        } else {
            inOutColor.setRedF(inOutColor.redF() / alpha);
            inOutColor.setGreenF(inOutColor.greenF() / alpha);
            inOutColor.setBlueF(inOutColor.blueF() / alpha);
        }

        if (enableColor) {
            // vec3 hsv = convertRGB2HSV(gl_FragColor.xyz);
            QColor hsv = inOutColor.toHsv();

            // this code forces grayscale values to be slightly saturated
            // so that some slight change of hue will be visible
            // const float minLightness = 0.11 / 2.0;
            const float minV = 0.11f / 2.0f;
            // const float minSaturation = 0.09;
            const float minS = 0.09f;
            // if (hsv.z < minLightness) hsv = vec3(0.0, 1.0, minLightness);
            if (hsv.valueF() < minV) {
                hsv.setHsvF(0.0f, 1.0f, minV);
                // else if (hsv.y < minSaturation) hsv = vec3(0.0, minSaturation, hsv.z);
            } else if (hsv.saturationF() < minS) {
                hsv.setHsvF(0.0f, minS, hsv.valueF());
            }

            // hsv.x = mod(hsv.x + u_color, 1.0);
            // if (hsv.x < 0.0) hsv.x += 1.0;
            float hue = std::fmod(uniforms[ShaderManager::Effect::Color] + hsv.hueF(), 1.0f);

            if (hue < 0.0f)
                hue += 1.0f;

            hsv.setHsvF(hue, hsv.saturationF(), hsv.valueF());

            // gl_FragColor.rgb = convertHSV2RGB(hsl);
            inOutColor = hsv.toRgb();
        }

        if (enableBrightness) {
            const float brightness = uniforms[ShaderManager::Effect::Brightness] * 255.0f;
            // gl_FragColor.rgb = clamp(gl_FragColor.rgb + vec3(u_brightness), vec3(0), vec3(1));
            inOutColor.setRed(std::clamp(inOutColor.red() + brightness, 0.0f, 255.0f));
            inOutColor.setGreen(std::clamp(inOutColor.green() + brightness, 0.0f, 255.0f));
            inOutColor.setBlue(std::clamp(inOutColor.blue() + brightness, 0.0f, 255.0f));
        }

        // gl_FragColor.rgb *= gl_FragColor.a + epsilon;
        // Now we're doing the reverse, premultiplying by the alpha once again.
        inOutColor.setRedF(inOutColor.redF() * alpha);
        inOutColor.setGreenF(inOutColor.greenF() * alpha);
        inOutColor.setBlueF(inOutColor.blueF() * alpha);

        // Restore alpha
        inOutColor.setAlphaF(alpha);
    }

    const float ghost = uniforms[ShaderManager::Effect::Ghost];

    if (ghost != 1) {
        // gl_FragColor *= u_ghost
        inOutColor.setRedF(inOutColor.redF() * ghost);
        inOutColor.setGreenF(inOutColor.greenF() * ghost);
        inOutColor.setBlueF(inOutColor.blueF() * ghost);
        inOutColor.setAlphaF(inOutColor.alphaF() * ghost);
    }

    return inOutColor.rgba();
}

void EffectTransform::transformPoint(ShaderManager::Effect effectMask, const std::unordered_map<ShaderManager::Effect, double> &effectValues, const QSize &size, const QVector2D &vec, QVector2D &dst)
{
    // https://github.com/scratchfoundation/scratch-render/blob/e075e5f5ebc95dec4a2718551624ad587c56f0a6/src/EffectTransform.js#L128-L194
    dst = vec;

    std::unordered_map<ShaderManager::Effect, float> uniforms;
    ShaderManager::getUniformValuesForEffects(effectValues, uniforms);

    if ((effectMask & ShaderManager::Effect::Mosaic) != 0) {
        // texcoord0 = fract(u_mosaic * texcoord0);
        const float mosaic = uniforms[ShaderManager::Effect::Mosaic];
        dst.setX(fract(mosaic * dst.x()));
        dst.setY(fract(mosaic * dst.y()));
    }

    if ((effectMask & ShaderManager::Effect::Pixelate) != 0) {
        // vec2 pixelTexelSize = u_skinSize / u_pixelate;
        const float pixelate = uniforms[ShaderManager::Effect::Pixelate];
        const float texelX = size.width() / pixelate;
        const float texelY = size.height() / pixelate;
        // texcoord0 = (floor(texcoord0 * pixelTexelSize) + kCenter) /
        //   pixelTexelSize;
        dst.setX((std::floor(dst.x() * texelX) + CENTER_X) / texelX);
        dst.setY((std::floor(dst.y() * texelY) + CENTER_Y) / texelY);
    }

    if ((effectMask & ShaderManager::Effect::Whirl) != 0) {
        const float whirl = uniforms[ShaderManager::Effect::Whirl];
        // const float kRadius = 0.5;
        const float RADIUS = 0.5f;
        // vec2 offset = texcoord0 - kCenter;
        const float offsetX = dst.x() - CENTER_X;
        const float offsetY = dst.y() - CENTER_Y;
        // float offsetMagnitude = length(offset);
        const float offsetMagnitude = std::sqrt(std::pow(offsetX, 2.0f) + std::pow(offsetY, 2.0f));
        // float whirlFactor = max(1.0 - (offsetMagnitude / kRadius), 0.0);
        const float whirlFactor = std::max(1.0f - (offsetMagnitude / RADIUS), 0.0f);
        // float whirlActual = u_whirl * whirlFactor * whirlFactor;
        const float whirlActual = whirl * whirlFactor * whirlFactor;
        // float sinWhirl = sin(whirlActual);
        const float sinWhirl = std::sin(whirlActual);
        // float cosWhirl = cos(whirlActual);
        const float cosWhirl = std::cos(whirlActual);
        // mat2 rotationMatrix = mat2(
        //     cosWhirl, -sinWhirl,
        //     sinWhirl, cosWhirl
        // );
        const float rot1 = cosWhirl;
        const float rot2 = -sinWhirl;
        const float rot3 = sinWhirl;
        const float rot4 = cosWhirl;

        // texcoord0 = rotationMatrix * offset + kCenter;
        dst.setX((rot1 * offsetX) + (rot3 * offsetY) + CENTER_X);
        dst.setY((rot2 * offsetX) + (rot4 * offsetY) + CENTER_Y);
    }

    if ((effectMask & ShaderManager::Effect::Fisheye) != 0) {
        const float fisheye = uniforms[ShaderManager::Effect::Fisheye];
        // vec2 vec = (texcoord0 - kCenter) / kCenter;
        const float vX = (dst.x() - CENTER_X) / CENTER_X;
        const float vY = (dst.y() - CENTER_Y) / CENTER_Y;
        // float vecLength = length(vec);
        const float vLength = std::sqrt((vX * vX) + (vY * vY));
        // float r = pow(min(vecLength, 1.0), u_fisheye) * max(1.0, vecLength);
        const float r = std::pow(std::min(vLength, 1.0f), fisheye) * std::max(1.0f, vLength);
        // vec2 unit = vec / vecLength;
        const float unitX = vX / vLength;
        const float unitY = vY / vLength;
        // texcoord0 = kCenter + r * unit * kCenter;
        dst.setX(CENTER_X + (r * unitX * CENTER_X));
        dst.setY(CENTER_Y + (r * unitY * CENTER_Y));
    }
}
