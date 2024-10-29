// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QVector2D>

#include "effecttransform.h"

using namespace scratchcpprender;

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

void EffectTransform::transformPoint(ShaderManager::Effect effectMask, const std::unordered_map<ShaderManager::Effect, double> &effectValues, const QVector2D &vec, QVector2D &dst)
{
    // TODO: Implement remaining effects
    dst = vec;
}
