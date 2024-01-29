// Ported from https://github.com/scratchfoundation/scratch-render/blob/4090e62e8abf427e55c83448da9b0df26120d2fb/src/shaders/sprite.frag

#ifdef ENABLE_color
uniform float u_color;
#endif // ENABLE_color

#ifdef ENABLE_brightness
uniform float u_brightness;
#endif // ENABLE_brightness

#ifdef ENABLE_ghost
uniform float u_ghost;
#endif // ENABLE_ghost

in vec2 v_texCoord;
//out vec4 FragColor;
uniform sampler2D u_skin;

// Add this to divisors to prevent division by 0, which results in NaNs propagating through calculations.
// Smaller values can cause problems on some mobile devices.
const float epsilon = 1e-3;

#if defined(ENABLE_color)
// Branchless color conversions based on code from:
// http://www.chilliant.com/rgb2hsv.html by Ian Taylor
// Based in part on work by Sam Hocevar and Emil Persson
// See also: https://en.wikipedia.org/wiki/HSL_and_HSV#Formal_derivation


// Convert an RGB color to Hue, Saturation, and Value.
// All components of input and output are expected to be in the [0,1] range.
vec3 convertRGB2HSV(vec3 rgb)
{
	// Hue calculation has 3 cases, depending on which RGB component is largest, and one of those cases involves a "mod"
	// operation. In order to avoid that "mod" we split the M==R case in two: one for G<B and one for B>G. The B>G case
	// will be calculated in the negative and fed through abs() in the hue calculation at the end.
	// See also: https://en.wikipedia.org/wiki/HSL_and_HSV#Hue_and_chroma
	const vec4 hueOffsets = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);

	// temp1.xy = sort B & G (largest first)
	// temp1.z = the hue offset we'll use if it turns out that R is the largest component (M==R)
	// temp1.w = the hue offset we'll use if it turns out that R is not the largest component (M==G or M==B)
	vec4 temp1 = rgb.b > rgb.g ? vec4(rgb.bg, hueOffsets.wz) : vec4(rgb.gb, hueOffsets.xy);

	// temp2.x = the largest component of RGB ("M" / "Max")
	// temp2.yw = the smaller components of RGB, ordered for the hue calculation (not necessarily sorted by magnitude!)
	// temp2.z = the hue offset we'll use in the hue calculation
	vec4 temp2 = rgb.r > temp1.x ? vec4(rgb.r, temp1.yzx) : vec4(temp1.xyw, rgb.r);

	// m = the smallest component of RGB ("min")
	float m = min(temp2.y, temp2.w);

	// Chroma = M - m
	float C = temp2.x - m;

	// Value = M
	float V = temp2.x;

	return vec3(
		abs(temp2.z + (temp2.w - temp2.y) / (6.0 * C + epsilon)), // Hue
		C / (temp2.x + epsilon), // Saturation
		V); // Value
}

vec3 convertHue2RGB(float hue)
{
	float r = abs(hue * 6.0 - 3.0) - 1.0;
	float g = 2.0 - abs(hue * 6.0 - 2.0);
	float b = 2.0 - abs(hue * 6.0 - 4.0);
	return clamp(vec3(r, g, b), 0.0, 1.0);
}

vec3 convertHSV2RGB(vec3 hsv)
{
	vec3 rgb = convertHue2RGB(hsv.x);
	float c = hsv.z * hsv.y;
	return rgb * c + hsv.z - c;
}
#endif // ENABLE_color

const vec2 kCenter = vec2(0.5, 0.5);

void main()
{
    vec4 texColor = texture2D(u_skin, v_texCoord);

    #ifdef ENABLE_color
    {
        vec3 hsv = convertRGB2HSV(texColor.rgb);

        // Force grayscale values to be slightly saturated
        const float minLightness = 0.11 / 2.0;
        const float minSaturation = 0.09;
        if (hsv.z < minLightness) hsv = vec3(0.0, 1.0, minLightness);
        else if (hsv.y < minSaturation) hsv = vec3(0.0, minSaturation, hsv.z);

        hsv.x = mod(hsv.x + u_color, 1.0);
        if (hsv.x < 0.0) hsv.x += 1.0;

        texColor.rgb = convertHSV2RGB(hsv);
    }
    #endif // ENABLE_color

    #ifdef ENABLE_brightness
    texColor.rgb = clamp(texColor.rgb + vec3(u_brightness), vec3(0), vec3(1));
    #endif // ENABLE_brightness

    #ifdef ENABLE_ghost
    texColor *= u_ghost;
    #endif // ENABLE_ghost

    // Set RGB components to zero if the color is fully transparent
    // This is a workaround for rendering issues when alpha is zero
    if(texColor.a == 0.0)
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    else
        gl_FragColor = texColor;
}
