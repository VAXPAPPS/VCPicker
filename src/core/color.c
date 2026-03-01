#include "color.h"
#include <stdio.h>
#include <math.h>

#define MIN_3(a, b, c) (fmin(a, fmin(b, c)))
#define MAX_3(a, b, c) (fmax(a, fmax(b, c)))

VaxpColorHSL vaxp_color_rgb_to_hsl(const VaxpColorRGB *rgb) {
    VaxpColorHSL hsl = {0, 0, 0, rgb->a};
    double r = rgb->r, g = rgb->g, b = rgb->b;
    double min = MIN_3(r, g, b);
    double max = MAX_3(r, g, b);
    double delta = max - min;

    hsl.l = (max + min) / 2.0;

    if (delta == 0) {
        hsl.h = 0;
        hsl.s = 0;
    } else {
        hsl.s = hsl.l < 0.5 ? delta / (max + min) : delta / (2.0 - max - min);

        if (max == r)
            hsl.h = (g - b) / delta + (g < b ? 6.0 : 0.0);
        else if (max == g)
            hsl.h = (b - r) / delta + 2.0;
        else
            hsl.h = (r - g) / delta + 4.0;
        hsl.h *= 60.0;
    }
    return hsl;
}

VaxpColorHSV vaxp_color_rgb_to_hsv(const VaxpColorRGB *rgb) {
    VaxpColorHSV hsv = {0, 0, 0, rgb->a};
    double r = rgb->r, g = rgb->g, b = rgb->b;
    double min = MIN_3(r, g, b);
    double max = MAX_3(r, g, b);
    double delta = max - min;

    hsv.v = max;

    if (delta == 0) {
        hsv.h = 0;
        hsv.s = 0;
    } else {
        hsv.s = delta / max;

        if (max == r)
            hsv.h = (g - b) / delta + (g < b ? 6.0 : 0.0);
        else if (max == g)
            hsv.h = (b - r) / delta + 2.0;
        else
            hsv.h = (r - g) / delta + 4.0;
        hsv.h *= 60.0;
    }
    return hsv;
}

VaxpColorCMYK vaxp_color_rgb_to_cmyk(const VaxpColorRGB *rgb) {
    VaxpColorCMYK cmyk = {0, 0, 0, 0, rgb->a};
    double r = rgb->r, g = rgb->g, b = rgb->b;
    double max = MAX_3(r, g, b);
    
    cmyk.k = 1.0 - max;
    if (cmyk.k < 1.0) {
        cmyk.c = (1.0 - r - cmyk.k) / (1.0 - cmyk.k);
        cmyk.m = (1.0 - g - cmyk.k) / (1.0 - cmyk.k);
        cmyk.y = (1.0 - b - cmyk.k) / (1.0 - cmyk.k);
    }
    return cmyk;
}

gchar* vaxp_color_rgb_to_hex(const VaxpColorRGB *rgb) {
    int r = (int)(rgb->r * 255.0 + 0.5);
    int g = (int)(rgb->g * 255.0 + 0.5);
    int b = (int)(rgb->b * 255.0 + 0.5);
    return g_strdup_printf("#%02X%02X%02X", r, g, b);
}

static double hue_to_rgb(double p, double q, double t) {
    if(t < 0.0) t += 1.0;
    if(t > 1.0) t -= 1.0;
    if(t < 1.0/6.0) return p + (q - p) * 6.0 * t;
    if(t < 1.0/2.0) return q;
    if(t < 2.0/3.0) return p + (q - p) * (2.0/3.0 - t) * 6.0;
    return p;
}

VaxpColorRGB vaxp_color_hsl_to_rgb(const VaxpColorHSL *hsl) {
    VaxpColorRGB rgb = {0, 0, 0, hsl->a};
    if (hsl->s == 0.0) {
        rgb.r = rgb.g = rgb.b = hsl->l; // achromatic
    } else {
        double q = hsl->l < 0.5 ? hsl->l * (1.0 + hsl->s) : hsl->l + hsl->s - hsl->l * hsl->s;
        double p = 2.0 * hsl->l - q;
        double hk = hsl->h / 360.0;
        rgb.r = hue_to_rgb(p, q, hk + 1.0/3.0);
        rgb.g = hue_to_rgb(p, q, hk);
        rgb.b = hue_to_rgb(p, q, hk - 1.0/3.0);
    }
    return rgb;
}

VaxpColorRGB vaxp_color_hex_to_rgb(const gchar *hex) {
    VaxpColorRGB rgb = {0, 0, 0, 1.0};
    if (!hex || hex[0] != '#') return rgb;
    
    unsigned int r = 0, g = 0, b = 0;
    if (sscanf(hex + 1, "%02x%02x%02x", &r, &g, &b) == 3) {
        rgb.r = r / 255.0;
        rgb.g = g / 255.0;
        rgb.b = b / 255.0;
    }
    return rgb;
}
