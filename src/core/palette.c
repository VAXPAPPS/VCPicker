#include "palette.h"
#include <math.h>

VaxpColorRGB vaxp_palette_get_complementary(const VaxpColorRGB *base) {
    VaxpColorHSL hsl = vaxp_color_rgb_to_hsl(base);
    hsl.h += 180.0;
    if (hsl.h >= 360.0) hsl.h -= 360.0;
    return vaxp_color_hsl_to_rgb(&hsl);
}

VaxpColorRGB* vaxp_palette_get_analogous(const VaxpColorRGB *base) {
    VaxpColorRGB *analogous = g_new(VaxpColorRGB, 2);
    VaxpColorHSL hsl = vaxp_color_rgb_to_hsl(base);
    
    VaxpColorHSL h1 = hsl;
    h1.h += 30.0;
    if (h1.h >= 360.0) h1.h -= 360.0;
    
    VaxpColorHSL h2 = hsl;
    h2.h -= 30.0;
    if (h2.h < 0.0) h2.h += 360.0;
    
    analogous[0] = vaxp_color_hsl_to_rgb(&h1);
    analogous[1] = vaxp_color_hsl_to_rgb(&h2);
    
    return analogous;
}

static double rel_luminance(const VaxpColorRGB *c) {
    double rs = c->r <= 0.03928 ? c->r / 12.92 : pow((c->r + 0.055) / 1.055, 2.4);
    double gs = c->g <= 0.03928 ? c->g / 12.92 : pow((c->g + 0.055) / 1.055, 2.4);
    double bs = c->b <= 0.03928 ? c->b / 12.92 : pow((c->b + 0.055) / 1.055, 2.4);
    return 0.2126 * rs + 0.7152 * gs + 0.0722 * bs;
}

double vaxp_color_contrast_ratio(const VaxpColorRGB *c1, const VaxpColorRGB *c2) {
    double l1 = rel_luminance(c1);
    double l2 = rel_luminance(c2);
    if (l1 > l2) {
        return (l1 + 0.05) / (l2 + 0.05);
    } else {
        return (l2 + 0.05) / (l1 + 0.05);
    }
}

gboolean vaxp_color_passes_aa(double ratio) {
    return ratio >= 4.5;
}

gboolean vaxp_color_passes_aaa(double ratio) {
    return ratio >= 7.0;
}
